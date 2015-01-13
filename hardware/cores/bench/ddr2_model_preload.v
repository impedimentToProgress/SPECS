// File intended to be included in the generate statement for each DDR2 part.
// The following loads a vmem file, "sram.vmem" by default, into the SDRAM.

// Wait until the DDR memory is initialised, and then magically load it
@(posedge dut.xilinx_ddr2_0.xilinx_ddr2_if0.phy_init_done);

$display("%t: Loading DDR2", $time);

// Since DDR2 consists of 4 16-bit wide modules that are loaded in 8-wide bursts
// we load the vmem file into a temporary location that is contiguous and
// loadable using readmemh
$readmemh("sram.vmem", program_array);

// Now transfer the data from the temporary location to the DDR2 model's memory
// Each DDR2 module takes 16-bits of data, four modules in parallel deliver 64-bits at once
// Construct the burst line (BL_MAX(8)*module data width(16-bits)=128-bits)
// Start on word0 (modules 0 and 1) or word1 (modules 2 and 3)
for(program_word_ptr = (i/2); program_word_ptr < (32'h08010000 >> 2); program_word_ptr = program_word_ptr + 2)
  begin
     // Modules 0 and 2 get low bits while modules 1 and 3 get high bits of the word
     ddr2_ram_mem_line[15:0] = program_array[program_word_ptr][15 + ((i%2)*16):((i%2)*16)];

     // Skip to next pair of words in program_array
     program_word_ptr = program_word_ptr + 2;
     ddr2_ram_mem_line[31:16] = program_array[program_word_ptr][15 + ((i%2)*16):((i%2)*16)];
     
     program_word_ptr = program_word_ptr + 2;
     ddr2_ram_mem_line[47:32] = program_array[program_word_ptr][15 + ((i%2)*16):((i%2)*16)];
     
     program_word_ptr = program_word_ptr + 2;
     ddr2_ram_mem_line[63:48] = program_array[program_word_ptr][15 + ((i%2)*16):((i%2)*16)];
     
     program_word_ptr = program_word_ptr + 2;
     ddr2_ram_mem_line[79:64] = program_array[program_word_ptr][15 + ((i%2)*16):((i%2)*16)];
     
     program_word_ptr = program_word_ptr + 2;
     ddr2_ram_mem_line[95:80] = program_array[program_word_ptr][15 + ((i%2)*16):((i%2)*16)];
     
     program_word_ptr = program_word_ptr + 2;
     ddr2_ram_mem_line[111:96] = program_array[program_word_ptr][15 + ((i%2)*16):((i%2)*16)];
     
     program_word_ptr = program_word_ptr + 2;
     ddr2_ram_mem_line[127:112] = program_array[program_word_ptr][15 + ((i%2)*16):((i%2)*16)];
     
     // Mask off to get burst line address
     // 8 X 16-bit burst x 4 modules = 512 bits = 16 32-bit pieces per burst
     burst_address = program_word_ptr & ~32'd15;
     
     // Convert from word address to 64-bit address (total size of DDR2 data interface and size of each item in a burst)
     burst_address = burst_address >> 1;

     // Put this assembled line into the DDR2 using its memory writing TASK
     // 2 bank bits+13 row bits+10 col bits X (16 data bits X 4 modules / 8-bits per byte)     
     u_mem0.memory_write(burst_address[2+13+10-1:13+10], burst_address[13+10-1:10], burst_address[10-1:0], ddr2_ram_mem_line);

     // Skip area not used by the program and start loading the IIE handler
     if(program_word_ptr >= (32'h00400000 >> 2) && program_word_ptr < (32'h08000000 >> 2))begin
	program_word_ptr = (32'h08000000 >> 2) - 2 + (i/2);
     end
  end // for (program_word_ptr = 0 ; program_word_ptr < ...

$display("(%t) * DDR2 RAM %1d preloaded",$time, i);
