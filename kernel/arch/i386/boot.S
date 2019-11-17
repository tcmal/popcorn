# Multiboot header
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Initial stack
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Our initial page directory & page table
.section .bss, "aw", @nobits
	.align 4096
boot_page_directory:
	.skip 4096
boot_page_table1:
	.skip 4096

.section .text
.global _start
.type _start, @function
_start:
	# Physical address of boot_page_table1.
	movl $(boot_page_table1 - 0xC0000000), %edi
	
	# First address to map is address 0.
	movl $0, %esi
	
	# Map 1023 pages. The 1024th will be the VGA text buffer.
	movl $1023, %ecx

	# Loop to get the starting address of our first page
next_page:
	# Exit condition: esi >= physical_kernel_start
	cmpl $(_kernel_start - 0xC0000000), %esi
	jge map_rest # Map everything after this

	# Size of page is 4096 bytes.
	addl $4096, %esi
	# Size of entries in boot_page_table1 is 4 bytes.
	addl $4, %edi
	# Loop to the next entry if we haven't finished.
	loop next_page

map_rest:
	# Map the rest of the kernel

	# Exit condition: esi >= physical_kernel_end
	cmpl $(_kernel_end - 0xC0000000), %esi
	jge kernel_mapped

	# Map physical address as "present, writable". Note that this maps
	# .text and .rodata as writable. Mind security and map them as non-writable.
	movl %esi, %edx
	orl $0x003, %edx
	movl %edx, (%edi)

	# Size of page is 4096 bytes.
	addl $4096, %esi
	# Size of entries in boot_page_table1 is 4 bytes.
	addl $4, %edi

	loop map_rest

kernel_mapped:

	# Map VGA video memory to 0xC03FF000 as "present, writable".
	movl $(0x000B8000 | 0x003), boot_page_table1 - 0xC0000000 + 1023 * 4

	# To start with map at both PD[0] (0x0 to 0x3FFFFF) and PD[768] (0xC0000000 to 0xC03FFFFF)
	# If we didn't map at the first, we'd immediately pagefault because we're in unmapped memory.
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
	movl $(boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

	# Use our page directory
	movl $(boot_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	# Enable paging and the write-protect bit.
	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Enter our higher half
	lea setup_done, %ecx
	jmp *%ecx

setup_done:
	# Now in the higher half.

	# Get rid of our old mapping
	movl $0, boot_page_directory + 0

	# Force reload.
	movl %cr3, %ecx
	movl %ecx, %cr3

	# Set up the stack.
	mov $stack_top, %esp

	# Global constructors.
	call _init

	# Main kernel
	call kernel_main

	# Infinite loop if dead.
	cli
1:	hlt
	jmp 1b