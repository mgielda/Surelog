// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

class core_ibex_base_test extends uvm_test;

  core_ibex_env                                   env;
  core_ibex_env_cfg                               cfg;
  virtual clk_if                                  clk_vif;
  virtual core_ibex_dut_probe_if                  dut_vif;
  virtual core_ibex_csr_if                        csr_vif;
  mem_model_pkg::mem_model                        mem;
  core_ibex_vseq                                  vseq;
  int unsigned                                    timeout_in_cycles = 10000000;
  // If no signature_addr handshake functionality is desired between the testbench and the generated
  // code, the test will wait for the specifield number of cycles before starting stimulus
  // sequences (irq and debug)
  int unsigned                                    stimulus_delay = 800;
  bit[ibex_mem_intf_agent_pkg::DATA_WIDTH-1:0]    signature_data_q[$];
  bit[ibex_mem_intf_agent_pkg::DATA_WIDTH-1:0]    signature_data;
  uvm_tlm_analysis_fifo #(ibex_mem_intf_seq_item) item_collected_port;
  uvm_tlm_analysis_fifo #(irq_seq_item)           irq_collected_port;
  uvm_phase                                       run;

  `uvm_component_utils(core_ibex_base_test)

  function new(string name="", uvm_component parent=null);
    core_ibex_report_server ibex_report_server;
    super.new(name, parent);
    ibex_report_server = new();
    uvm_report_server::set_server(ibex_report_server);
    item_collected_port = new("item_collected_port_test", this);
    irq_collected_port  = new("irq_collected_port_test", this);
  endfunction

  virtual function void build_phase(uvm_phase phase);
    super.build_phase(phase);
    $value$plusargs("timeout_in_cycles=%0d", timeout_in_cycles);
    if (!uvm_config_db#(virtual clk_if)::get(null, "", "clk_if", clk_vif)) begin
      `uvm_fatal(get_full_name(), "Cannot get clk_if")
    end
    if (!uvm_config_db#(virtual core_ibex_dut_probe_if)::get(null, "", "dut_if", dut_vif)) begin
      `uvm_fatal(get_full_name(), "Cannot get dut_if")
    end
    if (!uvm_config_db#(virtual core_ibex_csr_if)::get(null, "", "csr_if", csr_vif)) begin
      `uvm_fatal(get_full_name(), "Cannot get csr_if")
    end
    env = core_ibex_env::type_id::create("env", this);
    cfg = core_ibex_env_cfg::type_id::create("cfg", this);
    uvm_config_db#(core_ibex_env_cfg)::set(this, "*", "cfg", cfg);
    mem = mem_model_pkg::mem_model#()::type_id::create("mem");
    // Create virtual sequence and assign memory handle
    vseq = core_ibex_vseq::type_id::create("vseq");
    vseq.mem = mem;
    vseq.cfg = cfg;
  endfunction

  virtual function void connect_phase(uvm_phase phase);
    super.connect_phase(phase);
    env.data_if_slave_agent.monitor.item_collected_port.connect(
      this.item_collected_port.analysis_export);
    if (cfg.enable_irq_seq) begin
      env.irq_agent.monitor.irq_port.connect(this.irq_collected_port.analysis_export);
    end
  endfunction

  virtual task run_phase(uvm_phase phase);
    phase.raise_objection(this);
    run = phase;
    dut_vif.fetch_enable = 1'b0;
    clk_vif.wait_clks(100);
    load_binary_to_mem();
    dut_vif.fetch_enable = 1'b1;
    send_stimulus();
    wait_for_test_done();
    phase.drop_objection(this);
  endtask

  virtual function void report_phase(uvm_phase phase);
    super.report_phase(phase);
  endfunction

  virtual task send_stimulus();
    vseq.start(env.vseqr);
  endtask

  virtual task check_perf_stats();
  endtask

  function void load_binary_to_mem();
    string      bin;
    bit [7:0]   r8;
    bit [31:0]  addr = `BOOT_ADDR;
    int         f_bin;
    void'($value$plusargs("bin=%0s", bin));
    if (bin == "")
      `uvm_fatal(get_full_name(), "Please specify test binary by +bin=binary_name")
    `uvm_info(get_full_name(), $sformatf("Running test : %0s", bin), UVM_LOW)
    f_bin = $fopen(bin,"rb");
    if (!f_bin)
      `uvm_fatal(get_full_name(), $sformatf("Cannot open file %0s", bin))
    while ($fread(r8,f_bin)) begin
      `uvm_info(`gfn, $sformatf("Init mem [0x%h] = 0x%0h", addr, r8), UVM_FULL)
      mem.write(addr, r8);
      addr++;
    end
  endfunction

  virtual task wait_for_test_done();
    fork
      begin
        wait (dut_vif.ecall === 1'b1);
        vseq.stop();
        `uvm_info(`gfn, "ECALL instruction is detected, test done", UVM_LOW)
        // De-assert fetch enable to finish the test
        dut_vif.fetch_enable = 1'b0;
        fork
          check_perf_stats();
          // Wait some time for the remaining instruction to finish
          clk_vif.wait_clks(3000);
        join
      end
      begin
        clk_vif.wait_clks(timeout_in_cycles);
        `uvm_fatal(`gfn, "TEST TIMEOUT!!")
      end
    join_any
  endtask


  virtual task wait_for_mem_txn(input bit[ibex_mem_intf_agent_pkg::ADDR_WIDTH-1:0] ref_addr,
                                input signature_type_t ref_type);
    ibex_mem_intf_seq_item mem_txn;
    forever begin
      // The first write to this address is guaranteed to contain the signature type in bits [7:0]
      item_collected_port.get(mem_txn);
      if (mem_txn.addr == ref_addr && mem_txn.data[7:0] === ref_type &&
          mem_txn.read_write == WRITE) begin
        signature_data = mem_txn.data;
        case (ref_type)
          // The very first write to the signature address in every test is guaranteed to be a write
          // of CORE_STATUS, indicating the INITIALIZED state
          CORE_STATUS: begin
            signature_data_q.push_back(signature_data >> 8);
          end
          TEST_RESULT: begin
            signature_data_q.push_back(signature_data >> 8);
          end
          // The next 32 writes to the address are guaranteed to be a dump of all GPRs
          WRITE_GPR: begin
            for(int i = 0; i < 32; i++) begin
              do begin
                item_collected_port.get(mem_txn);
              end while(!(mem_txn.addr == ref_addr && mem_txn.read_write == WRITE));
              signature_data_q.push_back(mem_txn.data);
            end
          end
          // The next write to this address is guaranteed to be the data held in the CSR
          WRITE_CSR: begin
            signature_data_q.push_back(signature_data >> 8);
            do begin
              item_collected_port.get(mem_txn);
            end while (!(mem_txn.addr == ref_addr && mem_txn.read_write == WRITE));
            signature_data_q.push_back(mem_txn.data);
          end
          default: begin
            `uvm_fatal(`gfn,
              $sformatf("The data 0x%0h written to the signature address is formatted incorrectly.",
                        signature_data))
          end
        endcase
        return;
      end
    end
  endtask

  // API of various tasks wrapping wait_for_mem_txn, for various common functionalities that
  // might be needed for verification purposes.
  // Will be expanded as needed.

  // Gets the next CORE_STATUS signature write and compares it against the provided core_status
  // type, throws uvm_error on mismatch
  virtual task check_next_core_status(core_status_t core_status, string error_msg = "",
                                      int timeout = 9999999);
    run.raise_objection(this);
    fork
      begin
        wait_for_mem_txn(cfg.signature_addr, CORE_STATUS);
        signature_data = signature_data_q.pop_front();
        `DV_CHECK_EQ_FATAL(signature_data, core_status, error_msg);
      end
      begin : wait_timeout
        clk_vif.wait_clks(timeout);
        `uvm_fatal(`gfn,
                   $sformatf("Did not receive core_status 0x%0x within %0d cycle timeout period",
                   core_status, timeout))
      end
    join_any
    // Will only get here if we successfully beat the timeout period
    disable wait_timeout;
    run.drop_objection(this);
  endtask

  // Waits for a write to the address of the specified CSR and retrieves the csr data
  virtual task wait_for_csr_write(csr_num_e csr, int timeout = 9999999);
    bit [11:0] csr_addr;
    run.raise_objection(this);
    fork
      begin
        do begin
          wait_for_mem_txn(cfg.signature_addr, WRITE_CSR);
          csr_addr = signature_data_q.pop_front();
          signature_data = signature_data_q.pop_front();
        end while (csr_addr != csr);
      end
      begin : wait_timeout
        clk_vif.wait_clks(timeout);
        `uvm_fatal(`gfn,
                   $sformatf("Did not receive write to csr 0x%0x within %0d cycle timeout period",
                   csr, timeout))
      end
    join_any
    // Will only get here if we successfully beat the timeout period
    disable wait_timeout;
    run.drop_objection(this);
  endtask

  // Waits until the next time the given core_status is written to the signature address
  virtual task wait_for_core_status(core_status_t core_status);
    do begin
      wait_for_mem_txn(cfg.signature_addr, CORE_STATUS);
      signature_data = signature_data_q.pop_front();
    end while (signature_data != core_status);
  endtask

endclass