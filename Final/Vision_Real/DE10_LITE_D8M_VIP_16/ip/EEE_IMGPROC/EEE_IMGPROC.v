module EEE_IMGPROC(
	// global clock & reset
	clk,
	reset_n,
	
	// mm slave
	s_chipselect,
	s_read,
	s_write,
	s_readdata,
	s_writedata,
	s_address,

	// stream sink
	sink_data,
	sink_valid,
	sink_ready,
	sink_sop,
	sink_eop,
	
	// streaming source
	source_data,
	source_valid,
	source_ready,
	source_sop,
	source_eop,
	
	// conduit
	mode
	
);


// global clock & reset
input	clk;
input	reset_n;

// mm slave
input							s_chipselect;
input							s_read;
input							s_write;
output	reg	[31:0]	s_readdata;
input	[31:0]				s_writedata;
input	[2:0]					s_address;


// streaming sink
input	[23:0]            	sink_data;
input								sink_valid;
output							sink_ready;
input								sink_sop;
input								sink_eop;

// streaming source
output	[23:0]			  	   source_data;
output								source_valid;
input									source_ready;
output								source_sop;
output								source_eop;

// conduit export
input                         mode;

////////////////////////////////////////////////////////////////////////
//
parameter IMAGE_W = 11'd640;
parameter IMAGE_H = 11'd480;
parameter MESSAGE_BUF_MAX = 256;
parameter MSG_INTERVAL = 6;
parameter BB_COL_DEFAULT = 24'h00ff00;

wire  grey;
wire [7:0] red, green, blue, red_out, blue_out, green_out;
reg [8:0]   hue;
reg [7:0]   saturation, value;
wire         sop, eop, in_valid, out_ready;


reg [13:0] numerator_x60; 
reg [7:0] numerator, max_val, min_val, diff, sat_max, max_numer, denominator;
reg [2:0] rgb_sequence, rgb_sequence_n;

wire red_green = red > green ? 1:0;
wire red_blue = red > blue ? 1:0; 
wire green_blue = green > blue ? 1:0;

always @ (posedge clk)
begin
	if (sop)
	begin
		min_val <= 8'd0; max_val <= 8'd0; 
		numerator <= 8'd0;
		rgb_sequence <= 3'b010;
	end
	else 
	begin
	case ({red_green,red_blue,green_blue})
	3'b000:
			begin//BLUE GREEN RED
				min_val <= red; max_val <= blue; 
				numerator <= green - red; 
				rgb_sequence <= 3'b000;
			end
	3'b001:
			begin//GREEN BLUE RED
				min_val <= red; max_val <= green;
				numerator <= blue - red; 
				rgb_sequence <= 3'b001;
			end
	3'b011:
			begin//GREEN RED BLUE
				min_val <= blue; max_val <= green;
				numerator <= red - blue; 
				rgb_sequence <= 3'b011;
			end
	3'b100:
			begin//BLUE RED GREEN 
				min_val <= green; max_val <= blue;
				numerator <= red - green; 
				rgb_sequence <= 3'b100;
			end
	3'b110:
			begin//RED BLUE GREEN
				min_val <= green; max_val <= red;	
				numerator <= blue - green;
				rgb_sequence <= 3'b110;
			end
	3'b111:
			begin//RED GREEN BLUE
				min_val <= blue; max_val <= red;
				numerator <= green - blue;
				rgb_sequence <= 3'b111;
			end
	default
			begin
				min_val <= 8'd0; max_val <= 8'd0;
				numerator <= 8'd0;
				rgb_sequence <= 3'b010;
			end
	endcase
end
end

//--------------------------------------
       
always @ (posedge clk)
begin
	if (sop)
		begin
			numerator_x60 <= 14'd0;
			rgb_sequence_n <= 3'b010;
			diff <= 8'd0;
			max_numer <= 8'd0;
		end
	else
		begin
			numerator_x60 <= {numerator,6'b000000} - {numerator,2'b00};
			rgb_sequence_n <= rgb_sequence;
			diff <= max_val - min_val;
			max_numer <= max_val;
		end
end

always @ (*)
begin
	denominator = (diff > 8'd0) ? numerator_x60 / diff : 8'd240;  
end

always @ (posedge clk)
begin
	if (!sop)
	
		hue <= 9'd0;
	
	else 
	begin
	case (rgb_sequence_n)
//"

	3'b000:
			//BLUE GREEN RED
			hue <= 9'd240 - denominator;
			
	3'b001:
			//GREEN BLUE RED
			hue <= 9'd120 + denominator;
			
	3'b011:
			//GREEN RED BLUE
			hue <= 9'd120 - denominator;			
	3'b100:
			//BLUE RED GREEN 
			hue <= 9'd240 + denominator;
			
	3'b110:
			//RED BLUE GREEN
			hue <= 9'd360 - denominator;
			
			
	3'b111:
			//RED GREEN BLUE
			hue <= denominator;
			
	default
			hue <= 0;
	endcase
end
end

always @ (*)
begin
	sat_max = (max_numer > 8'd0)? {diff[7:0],8'b00000000} / max_numer : 8'd0; 
end
always@(posedge clk)
begin
	if (!sop)
      saturation <= 8'd0;
	else
	saturation <= sat_max;
end

always@(posedge clk)
begin
  if (!sop)
	  value <= 8'd0;
  else
  	value <= max_numer;
 end


//" [1]

//Last case statement written with inspiration from the methodology highlighted in the page referenced at the bottom

wire blue_hsv,green_hsv,red_hsv,orange_hsv,dblue_hsv;

assign green_hsv = (((hue[8:0] >  9'd130) && (hue[8:0] <  9'd165)) &&  ((saturation[7:0] > 8'd90) && (saturation[7:0] <  8'd255)) && ((value[7:0] >  8'd50) && (value[7:0] <  8'd255)));

assign red_hsv = (((hue[8:0] <=  9'd320) && (hue[8:0] >=  9'd290)) &&  ((saturation[7:0] <= 8'd255) && (saturation[7:0] >=  8'd90)) && ((value[7:0] >= 8'd50) && (value[7:0] <=  8'd255)));
 
assign blue_hsv = (((hue[8:0] >  9'd185) && (hue[8:0] <  9'd215)) &&  ((saturation[7:0] > 8'd90) && (saturation[7:0] <=  8'd255)) && ((value[7:0] >  8'd50) && (value[7:0] <=  8'd255)));

assign orange_hsv = (((hue[8:0] >  9'd30) && (hue[8:0] <  9'd60)) &&  ((saturation[7:0] > 8'd90) && (saturation[7:0] <=  8'd255)) && ((value[7:0] >  8'd50) && (value[7:0] <=  8'd255)));

assign dblue_hsv = (((hue[8:0] >  9'd100) && (hue[8:0] <  9'd250)) &&  ((saturation[7:0] > 8'd100) && (saturation[7:0] <=  8'd255)) && ((value[7:0] >  8'd0) && (value[7:0] <=  8'd50)));



wire red_detect,green_detect,blue_detect,orange_detect,dblue_detect;
assign green_detect = green_hsv && ~blue_hsv && ~red_hsv && ~orange_hsv && ~dblue_hsv;
assign red_detect = ~green_hsv && ~blue_hsv && red_hsv && ~orange_hsv && ~dblue_hsv;
assign blue_detect = blue_hsv && ~red_hsv && ~green_hsv && ~orange_hsv && ~dblue_hsv;
assign orange_detect = orange_hsv && blue_hsv && ~red_hsv && ~green_hsv && ~dblue_hsv;
 

 
 

// Find boundary of cursor box

// Highlight detected areas
//assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4


wire [23:0] red_high;
assign red_high  =  red_detect ? {8'd255,8'd0, 8'd0} : {grey, grey, grey}; // actually pink not red


wire [23:0] blue_high;
assign blue_high  =  blue_detect ? {8'd85, 8'd255, 8'd255} : {grey, grey, grey};


wire [23:0] green_high;
assign green_high  =  green_detect ? {8'd0, 8'd255, 8'd0}: {grey, grey, grey};


wire [23:0] orange_high;
assign orange_high  =  orange_detect ? {8'd255, 8'd20, 8'd35}: {grey, grey, grey};


wire [23:0] dblue_high;
assign dblue_high  =  dblue_detect ? {8'd75, 8'd85, 8'd95}: {grey, grey, grey};

// Show bounding box for each colour
wire [23:0] newimage_red;
wire bb_active_red;
assign bb_active_red = (x_red == left_red) | (x_red == right_red) | (y_red == top_red) | (y_red == bottom_red);
assign newimage_red = bb_active_red ? bb_col : red_high;

wire [23:0] newimage_green;
wire bb_active_green;
assign bb_active_green = (x_green == left_green) | (x_green == right_green) | (y_green == top_green) | (y_green == bottom_green);
assign newimage_green = bb_active_green ? bb_col : green_high;

wire [23:0] newimage_blue;
wire bb_active_blue;
assign bb_active_blue = (x_blue == left_blue) | (x_blue == right_blue) | (y_blue == top_blue) | (y_blue == bottom_blue);
assign newimage_blue = bb_active_blue ? bb_col : blue_high;


wire [23:0] newimage_orange;
wire bb_active_orange;
assign bb_active_orange = (x_orange == left_orange) | (x_orange == right_orange) | (y_orange == top_orange) | (y_orange == bottom_orange);
assign newimage_orange = bb_active_orange ? bb_col : orange_high;


wire [23:0] newimage_dblue;
wire bb_active_dblue;
assign bb_active_dblue = (x_dblue == left_dblue) | (x_dblue == right_dblue) | (y_dblue == top_dblue) | (y_dblue == bottom_dblue);
assign newimage_dblue = bb_active_dblue ? bb_col : dblue_high;

// Switch output pixels depending on mode switch
// Don't modify the start-of-packet word - it's a packet discriptor
// Don't modify data in non-video packets



assign  {red_out, green_out, blue_out} = (mode & ~sop & packet_video)  ? ( (newimage_red && ~newimage_green && ~newimage_blue && ~newimage_dblue && ~newimage_orange) ? {newimage_red[23:16],newimage_red[15:8],newimage_red[7:0]} : ((~newimage_red && newimage_green && ~newimage_blue && ~newimage_dblue && ~newimage_orange) ? {newimage_green[23:16],newimage_green[15:8],newimage_green[7:0]} : ( (~newimage_red && ~newimage_green && newimage_blue && ~newimage_dblue && ~newimage_orange) ? {newimage_blue[23:16],newimage_blue[15:8],newimage_blue[7:0]} :( (~newimage_red && ~newimage_green && ~newimage_blue && newimage_dblue && ~newimage_orange) ? {newimage_dblue[23:16],newimage_dblue[15:8],newimage_dblue[7:0]} :((~newimage_red && ~newimage_green && ~newimage_blue && ~newimage_dblue && newimage_orange) ? {newimage_orange[23:16],newimage_orange[15:8],newimage_orange[7:0]} : {grey,grey,grey} ) )))) : {red,green,blue};

//Count valid pixels to tget the image coordinates. Reset and detect packet type on Start of Packet.
reg [10:0] x_red, y_red, x_green, y_green, x_blue, y_blue, x_dblue, y_dblue, x_orange, y_orange;
reg packet_video;
always@(posedge clk) begin
	if (sop) begin
		x_red <= 11'h0;
		y_red <= 11'h0;
		x_green <= 11'h0;
		y_green <= 11'h0;
		x_blue <= 11'h0;
		y_blue <= 11'h0;
		x_orange <= 11'h0;
		y_orange <= 11'h0;
		x_dblue <= 11'h0;
		y_dblue <= 11'h0;
		packet_video <= (blue[3:0] == 3'h0);
	end
	else if (in_valid) begin
		if (x_red == IMAGE_W-1) begin
			x_red <= 11'h0;
			y_red <= y_red + 11'h1;
		end
		else begin
			x_red <= x_red + 11'h1;
		end
		if (x_blue == IMAGE_W-1) begin
			x_blue <= 11'h0;
			y_blue <= y_blue + 11'h1;
		end
		else begin
			x_blue <= x_blue + 11'h1;
		end
		if (x_green == IMAGE_W-1) begin
			x_green <= 11'h0;
			y_green <= y_green+ 11'h1;
		end
		else begin
			x_green <= x_green + 11'h1;
		end
		if (x_orange == IMAGE_W-1) begin
			x_orange <= 11'h0;
			y_orange <= y_orange+ 11'h1;
		end
		else begin
			x_orange <= x_orange + 11'h1;
		end
		if (x_dblue == IMAGE_W-1) begin
			x_dblue <= 11'h0;
			y_dblue <= y_dblue + 11'h1;
		end
		else begin
			x_dblue <= x_dblue + 11'h1;
		end
	end
end


//Find first and last red pixels
reg [10:0] x_min_red, y_min_red, x_max_red, y_max_red, x_min_green, y_min_green, x_max_green, y_max_green, x_min_blue, y_min_blue, x_max_blue, y_max_blue, x_min_orange, y_min_orange, x_max_orange, y_max_orange, x_min_dblue, y_min_dblue, x_max_dblue, y_max_dblue;
always@(posedge clk) begin
	if (red_detect & in_valid) begin	//Update bounds when the pixel is red(actually pink)
		if (x_red < x_min_red) x_min_red <= x_red;
		if (x_red > x_max_red) x_max_red <= x_red;
		if (y_red < y_min_red) y_min_red <= y_red;
		y_max_red <= y_red;
	end
	if (green_detect & in_valid) begin	//Update bounds when the pixel is green
		if (x_green < x_min_green) x_min_green <= x_green;
		if (x_green > x_max_green) x_max_green <= x_green;
		if (y_green < y_min_green) y_min_green <= y_green;
		y_max_green <= y_green;
	end
	if (blue_detect & in_valid) begin	//Update bounds when the pixel is blue
		if (x_blue < x_min_blue) x_min_blue <= x_blue;
		if (x_blue > x_max_blue) x_max_blue <= x_blue;
		if (y_blue < y_min_blue) y_min_blue <= y_blue;
		y_max_blue <= y_blue;
	end
	if (orange_detect & in_valid) begin	//Update bounds when the pixel is orange
		if (x_orange < x_min_orange) x_min_orange <= x_orange;
		if (x_orange > x_max_orange) x_max_orange <= x_orange;
		if (y_orange < y_min_orange) y_min_orange <= y_orange;
		y_max_orange <= y_orange;
	end
	if (dblue_detect & in_valid) begin	//Update bounds when the pixel is dblue/grey
		if (x_dblue < x_min_dblue) x_min_dblue <= x_dblue;
		if (x_dblue > x_max_dblue) x_max_dblue <= x_dblue;
		if (y_dblue < y_min_dblue) y_min_dblue <= y_dblue;
		y_max_dblue <= y_dblue;
	end
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_red <= IMAGE_W-11'h1;
		x_max_red <= 0;
		y_min_red <= IMAGE_H-11'h1;
		y_max_red <= 0;
		
		x_min_green <= IMAGE_W-11'h1;
		x_max_green <= 0;
		y_min_green <= IMAGE_H-11'h1;
		y_max_green <= 0;
		
		x_min_blue <= IMAGE_W-11'h1;
		x_max_blue <= 0;
		y_min_blue <= IMAGE_H-11'h1;
		y_max_blue <= 0;
		
		x_min_orange <= IMAGE_W-11'h1;
		x_max_orange <= 0;
		y_min_orange <= IMAGE_H-11'h1;
		y_max_orange <= 0;

		x_min_dblue <= IMAGE_W-11'h1;
		x_max_dblue <= 0;
		y_min_dblue <= IMAGE_H-11'h1;
		y_max_dblue <= 0;
	end
end

//Process bounding box at the end of the frame.
reg [3:0] msg_state;
reg [10:0] left_red, right_red, top_red, bottom_red, left_green, right_green, top_green, bottom_green, left_blue, right_blue, top_blue, bottom_blue, left_orange, right_orange, top_orange, bottom_orange, left_dblue, right_dblue, top_dblue, bottom_dblue;
reg [7:0] frame_count;
always@(posedge clk) begin
	if (eop & in_valid & packet_video) begin  //Ignore non-video packets
		
		//Latch edges for display overlay on next frame
		left_red <= x_min_red;
		right_red <= x_max_red;
		top_red <= y_min_red;
		bottom_red <= y_max_red;
		
		left_green <= x_min_green;
		right_green <= x_max_green;
		top_green <= y_min_green;
		bottom_green <= y_max_green;
		
		left_blue <= x_min_blue;
		right_blue <= x_max_blue;
		top_blue <= y_min_blue;
		bottom_blue <= y_max_blue;
		

		left_dblue <= x_min_dblue;
		right_dblue <= x_max_dblue;
		top_dblue <= y_min_dblue;
		bottom_dblue <= y_max_dblue;
		
		left_orange <= x_min_orange;
		right_orange <= x_max_orange;
		top_orange <= y_min_orange;
		bottom_orange <= y_max_orange;
		
		
		//Start message writer FSM once every MSG_INTERVAL frames, if there is room in the FIFO
		frame_count <= frame_count - 1;
		
		if (frame_count == 0 && msg_buf_size < MESSAGE_BUF_MAX - 3) begin
			msg_state <= 4'b0001;
			frame_count <= MSG_INTERVAL-1;
		end
	end
	
	//Cycle through message writer states once started
	if (msg_state != 4'b0000) msg_state <= msg_state + 4'b0001;

end
	
//Generate output messages for CPU
reg [31:0] msg_buf_in; 
wire [31:0] msg_buf_out;
reg msg_buf_wr;
wire msg_buf_rd, msg_buf_flush;
wire [7:0] msg_buf_size;
wire msg_buf_empty;

`define BOX_MSG_ID "RBB" //unclear as to what this means

always@(*) begin	//Write words to FIFO as state machine advances
	case(msg_state)
		4'b0000: begin
			msg_buf_in = 32'b0;
			msg_buf_wr = 1'b0;
		end
		4'b0001: begin
			msg_buf_in = `BOX_MSG_ID;	//Message ID
			msg_buf_wr = 1'b1;
		end
		4'b0010: begin
			msg_buf_in = {5'b0, x_min_red, 5'b0, y_min_red};	//Top left coordinate red
			msg_buf_wr = 1'b1;
		end
		4'b0011: begin
			msg_buf_in = {5'b0, x_max_red, 5'b0, y_max_red}; //Bottom right coordinate red
			msg_buf_wr = 1'b1;
		end
		4'b0100: begin
			msg_buf_in = {5'b0, x_min_green, 5'b0, y_min_green};	//Top left coordinate green
			msg_buf_wr = 1'b1;
		end
		4'b0101: begin
			msg_buf_in = {5'b0, x_max_green, 5'b0, y_max_green}; //Bottom right coordinate green
			msg_buf_wr = 1'b1;
		end
		4'b0110: begin
			msg_buf_in = {5'b0, x_min_blue, 5'b0, y_min_blue};	//Top left coordinate blue
			msg_buf_wr = 1'b1;
		end
		4'b0111: begin
			msg_buf_in = {5'b0, x_max_blue, 5'b0, y_max_blue}; //Bottom right coordinate blue
			msg_buf_wr = 1'b1;
		end
		4'b1000: begin
			msg_buf_in = {5'b0, x_min_dblue, 5'b0, y_min_dblue};	//Top left coordinate dark blue / grey
			msg_buf_wr = 1'b1;
		end
		4'b1001: begin
			msg_buf_in = {5'b0, x_max_dblue, 5'b0, y_max_dblue}; //Bottom right coordinate dark blue / grey
			msg_buf_wr = 1'b1;
		end
		3'b1010: begin
			msg_buf_in = {5'b0, x_min_orange, 5'b0, y_min_orange};	//Top left coordinate orange
			msg_buf_wr = 1'b1;
		end
		4'b1011: begin
			msg_buf_in = {5'b0, x_max_orange, 5'b0, y_max_orange}; //Bottom right coordinate orange 
			msg_buf_wr = 1'b1;
		end
	endcase
end


//Output message FIFO
MSG_FIFO	MSG_FIFO_inst (
	.clock (clk),
	.data (msg_buf_in),
	.rdreq (msg_buf_rd),
	.sclr (~reset_n | msg_buf_flush),
	.wrreq (msg_buf_wr),
	.q (msg_buf_out),
	.usedw (msg_buf_size),
	.empty (msg_buf_empty)
	);


//Streaming registers to buffer video signal
STREAM_REG #(.DATA_WIDTH(26)) in_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(sink_ready),
	.valid_out(in_valid),
	.data_out({red,green,blue,sop,eop}),
	.ready_in(out_ready),
	.valid_in(sink_valid),
	.data_in({sink_data,sink_sop,sink_eop})
);

STREAM_REG #(.DATA_WIDTH(26)) out_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(out_ready),
	.valid_out(source_valid),
	.data_out({source_data,source_sop,source_eop}),
	.ready_in(source_ready),
	.valid_in(in_valid),
	.data_in({red_out, green_out, blue_out, sop, eop})
);


/////////////////////////////////
/// Memory-mapped port		 /////
/////////////////////////////////

// Addresses
`define REG_STATUS    			0
`define READ_MSG    				1
`define READ_ID    				2
`define REG_BBCOL					3

//Status register bits
// 31:16 - unimplemented
// 15:8 - number of words in message buffer (read only)
// 7:5 - unused
// 4 - flush message buffer (write only - read as 0)
// 3:0 - unused


// Process write

reg  [7:0]   reg_status;
reg	[23:0]	bb_col;

always @ (posedge clk)
begin
	if (~reset_n)
	begin
		reg_status <= 8'b0;
		bb_col <= BB_COL_DEFAULT;
	end
	else begin
		if(s_chipselect & s_write) begin
		   if      (s_address == `REG_STATUS)	reg_status <= s_writedata[7:0];
		   if      (s_address == `REG_BBCOL)	bb_col <= s_writedata[23:0];
		end
	end
end


//Flush the message buffer if 1 is written to status register bit 4
assign msg_buf_flush = (s_chipselect & s_write & (s_address == `REG_STATUS) & s_writedata[4]);


// Process reads
reg read_d; //Store the read signal for correct updating of the message buffer

// Copy the requested word to the output port when there is a read.
always @ (posedge clk)
begin
   if (~reset_n) begin
	   s_readdata <= {32'b0};
		read_d <= 1'b0;
	end
	
	else if (s_chipselect & s_read) begin
		if   (s_address == `REG_STATUS) s_readdata <= {16'b0,msg_buf_size,reg_status};
		if   (s_address == `READ_MSG) s_readdata <= {msg_buf_out};
		if   (s_address == `READ_ID) s_readdata <= 32'h1234EEE2;
		if   (s_address == `REG_BBCOL) s_readdata <= {8'h0, bb_col};
	end
	
	read_d <= s_read;
end

//Fetch next word from message buffer after read from READ_MSG
assign msg_buf_rd = s_chipselect & s_read & ~read_d & ~msg_buf_empty & (s_address == `READ_MSG);
						


endmodule

// [1] - "rgb to hsv - Programmer Sought", Programmersought.com, 2021. [Online]. Available: https://www.programmersought.com/article/31054814412/. [Accessed: 21- Jun- 2021].