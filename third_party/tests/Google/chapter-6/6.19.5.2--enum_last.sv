/*
:name: enum_last
:description: enum last method tests
:should_fail: 0
:tags: 6.19.5.2
*/
module top();
	typedef enum {a, b, c, d} e;

	initial begin
		e val = a;
		val = val.last();
	end
endmodule
