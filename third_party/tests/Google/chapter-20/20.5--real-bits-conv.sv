/*
:name: realtobits_bitstoreal_functions
:description: $realtobits and $bitstoreal test
:should_fail: 0
:tags: 20.5
:type: simulation parsing
*/

module top();

initial begin
	$display(":assert: (%f == 12.45)", $bitstoreal($realtobits(12.45)));
end

endmodule
