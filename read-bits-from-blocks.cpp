int cur_pos = 0;  // global byte offset
int B = block_size();

void seek(int location) {
    cur_pos = location;
}

void read(char* out, int len) {
    int remaining = len;
    int out_offset = 0;

    while (remaining > 0) {
        int block_no = cur_pos / B;
        int offset_in_block = cur_pos % B;

        char block[B];
        block_seek(block_no);
        block_read(block_no, block);

        int bytes_to_copy = min(B - offset_in_block, remaining);
        memcpy(out + out_offset, block + offset_in_block, bytes_to_copy);

        cur_pos += bytes_to_copy;
        out_offset += bytes_to_copy;
        remaining -= bytes_to_copy;
    }
}

void write(const char* buf, int len) {
    int remaining = len;
    int buf_offset = 0;

    while (remaining > 0) {
        int block_no = cur_pos / B;
        int offset_in_block = cur_pos % B;

        char block[B];
        block_seek(block_no);
        block_read(block_no, block);   // needed for partial overwrite

        int bytes_to_write = min(B - offset_in_block, remaining);
        memcpy(block + offset_in_block, buf + buf_offset, bytes_to_write);

        block_seek(block_no);
        block_write(block_no, block);

        cur_pos += bytes_to_write;
        buf_offset += bytes_to_write;
        remaining -= bytes_to_write;
    }
}
