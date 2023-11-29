
#include <defs.h>
#include <stub.h>
#include <serial.h>
#include "hyperram.h"

#define HYPERRAM_CSR_LATENCY_FIXED_VARIABLE_MASK	(0x0020)
#define HYPERRAM_CSR_LATENCY_FIXED					(0x0020)
#define HYPERRAM_CSR_LATENCY_VARIABLE				(0x0000)
#define HYPERRAM_CSR_LATENCY_MUL_MASK				(0x0010)
#define HYPERRAM_CSR_LATENCY_DOUBLE					(0x0010)
#define HYPERRAM_CSR_LATENCY_SINGLE					(0x0000)
#define HYPERRAM_CSR_LATENCY_MASK					(0x000f)

static uint32_t xorshift32(uint32_t *state)
{
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	uint32_t x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;

	return x;
}

void hyperram_set_latency(uint8_t fixed, uint8_t cycles)
{
	uint16_t cfg = 0;
	uint32_t status = 0;

	print("\nChanging latency to: ");
	print_dec(cycles);
	print(" cycles");
	if (fixed)
	{
		print(", Fixed 2 times\n");
	}
	else
	{
		print(", Variable\n");
	}

	if ((cycles < 3) || (cycles > 6))
	{
		print("Invalid number requested\n");
		return;
	}

	// read chip configuration
	cfg = hyperram_reg(HYPERRAM_CHIP_CFG0);
	status = hyperram_csr_status;			// status, 1 -> timeout on read	
	if (status)
	{
		print("CFG0 read timeout\n");
		return;
	}

	// adjust CFG0 register
	cfg &= 0xff07;
	switch (cycles)
	{
		case 3: cfg |= 0x00e0; break;
		case 4: cfg |= 0x00f0; break;
		case 6: cfg |= 0x0010; break;
		default: break;
	}
	if (fixed)
	{
		cfg |= 0x0008;
	}
	hyperram_reg(HYPERRAM_CHIP_CFG0) = cfg;

	// reconfigure CSR inside driver
	cfg = cycles;
	cfg |= fixed ? 0x30 : 0x00;
	hyperram_csr_latency = cfg;
	if (hyperram_csr_latency != cfg)
	{
		print("Setting latency in CSR failed\n");
		return;
	}

	print("Latency set ok!\n");
}

void hyperram_read_csrs()
{
	uint16_t csr;

	// read CSRs inside HyperRAM driver
	csr = hyperram_csr_latency;
	print("Latency CSR: ");
	print_hex(csr, 4);
	print("\n");
	csr = hyperram_csr_tcsh;
	print("Tcsh CSR: ");
	print_hex(csr, 4);
	print("\n");
	csr = hyperram_csr_tpre_tpost;
	print("Tpre Tpost CSR: ");
	print_hex(csr, 4);
	print("\n");
	csr = hyperram_csr_trmax;
	print("Read timeout CSR: ");
	print_hex(csr, 4);
	print("\n");
	csr = hyperram_csr_status;
	print("Status CSR: ");
	print_hex(csr, 4);
	print("\n");
}

void hyperram_read_chip_id()
{
	uint16_t id;
	uint32_t status;

	id = hyperram_reg(HYPERRAM_CHIP_ID0);
	status = hyperram_csr_status;			// status, 1 -> timeout on read	
	if (status)
	{
		print("\nID0 read timeout\n");
		return;
	}
	print("\nID0: ");
	print_hex(id, 4);
	print("\nBits for Row Address: ");
	print_dec( ((id >> 8) & 0x0f) + 1 );
	print("\nBits for Column Address: ");
	print_dec( ((id >> 4) & 0x0f) + 1 );
	print("\nManufacturer: ");
	if ((id & 0x000f) == 1)
	{
		print("Cypress\n");
	}
	else
	{
		print("Unknown\n");
	}

	id = hyperram_reg(HYPERRAM_CHIP_ID1);
	status = hyperram_csr_status;			// status, 1 -> timeout on read	
	if (status)
	{
		print("ID1 read timeout\n");
		return;
	}
	print("ID1: ");
	print_hex(id, 4);
	print("\n");
}

// read chip configuration
void hyperram_read_chip_cfg()
{
	uint16_t cfg;
	uint32_t status;

	cfg = hyperram_reg(HYPERRAM_CHIP_CFG0);
	status = hyperram_csr_status;			// status, 1 -> timeout on read	
	if (status)
	{
		print("\nCFG0 read timeout\n");
		return;
	}
	print("\nCFG0: ");
	print_hex(cfg, 4);
	print("\nDrive strength: ");
	print_hex( (cfg >> 12) & 0x07, 2);
	print("\nLatency: ");
	switch ((cfg >> 4) & 0x0f)
	{
		case 0x0: print_dec(5); break;
		case 0x1: print_dec(6); break;
		case 0xe: print_dec(3); break;
		case 0xf: print_dec(4); break;
		default: print("Reserved!");
	}
	if (cfg & 0x0008)
	{
		print("\nFixed 2 times Latency");
	}
	else
	{
		print("\nVariable 1 or 2 times Latency");
	}
	if (cfg & 0x0004)
	{
		print("\nLegacy burst");
	}
	else
	{
		print("\nHybrid burst");		
	}
	print("\nBurst length: ");
	switch (cfg & 0x0003)
	{
		case 0: print_dec(128); break;
		case 1: print_dec(64); break;
		case 2: print_dec(16); break;
		case 3: print_dec(32); break;
		default: break;
	}
	print("\n");

	cfg = hyperram_reg(HYPERRAM_CHIP_CFG1);
	status = hyperram_csr_status;			// status, 1 -> timeout on read	
	if (status)
	{
		print("CFG1 read timeout\n");
		return;
	}
	print("CFG1: ");
	print_hex(cfg, 4);
	print("\nRefresh Interval: ");	
	switch (cfg & 0x0003)
	{
		case 0: print("2 times default"); break;
		case 1: print("4 times default"); break;
		case 2: print("default (IP 1us or I 4us)"); break;
		case 3: print("1.5 times default"); break;
		default: break;
	}
	print("\n");
	if ((cfg & 0x0003) != 2)
	{
		// make distributed refresh interval shorter (for low frequency acccess)
		hyperram_reg(HYPERRAM_CHIP_CFG1) = 2;

		cfg = hyperram_reg(HYPERRAM_CHIP_CFG1);
		status = hyperram_csr_status;			// status, 1 -> timeout on read 

		if (!status || ((cfg & 0x0003) != 2))
		{
			print("Setting default Distributed Refresh Interval failed!\n");
		}
		else
		{
			print("Distributed Refresh Interval set to default\n");
		}
	}
}

void hyperram_memtest()
{
	uint32_t cyc_count = 5;
	uint32_t stride = 256;
	uint32_t state;
	uint32_t read, write;

	print("Running hyperram memtest ");

	for (uint32_t i = 1; i <= cyc_count; i++) 
	{
		state = i;

		for (uint32_t word = 0; word < HYPERRAM_MEM_SIZE_WORDS; word += stride) 
		{
			hyperram_mem(word) = xorshift32(&state);
		}

		state = i;

		for (uint32_t word = 0; word < HYPERRAM_MEM_SIZE_WORDS; word += stride) 
		{
			read = hyperram_mem(word);
			write = xorshift32(&state);
			if (read != write) 
			{
				print(" ***FAILED WORD*** at ");
				print_hex(word, 8);
				print("\nWritten value: ");
				print_hex(write, 8);
				print("\nRead value: ");
				print_hex(read, 8);
				print("\n");

				return;
			}
		}

		print(".");
	}
}

void hyperram_write_read(uint32_t address, uint32_t data)
{
	volatile uint32_t value = 0;
	volatile uint32_t status = 0;

	// write
	hyperram_mem(address) = data;
	print("\nWriting to address: ");
	print_hex(address, 8);
	print("\nValue: ");
	print_hex(data, 8);	
	
	// read back	
	value = hyperram_mem(address);			// value
	status = hyperram_csr_status;			// status, 1 -> timeout on read
	print("\nRead Status: ");
	if (status)
	{
		print("TIMEOUT!\n");
		return;
	}
	print("OK!\n");

	print("Read data: ");
	print_hex(value, 8);	
	print("\n");
	if (value != data)
	{
		print("Write-read failed!\n");
		return;
	}
	print("Write-read ok!\n");
}

void hyperram_test()
{
	uint32_t pattern;
	volatile uint32_t value = 0;
	volatile uint32_t status = 0;

	print("\nHyperRAM test....\n");

	serial_getc();

	hyperram_read_csrs();

	hyperram_read_chip_id();

	hyperram_read_chip_cfg();

	hyperram_write_read(0x7ffff4, 0x12874562);
	
	hyperram_set_latency(1, 4);
	hyperram_write_read(0x1234, 0x00caffe0);

	hyperram_set_latency(0, 6);
	hyperram_write_read(0x456008, 0xdeadbeef);

	hyperram_set_latency(0, 4);
	hyperram_write_read(0x002c00, 0xbeefbeef);

	print("\nHyperRAM memtest....\n");

	serial_getc();

	hyperram_set_latency(1, 6);
	hyperram_memtest();

}
