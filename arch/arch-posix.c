#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>

#include "verbose.h"

bool disk_dev_open(disk_dev_t *dev, const char *path)
{
	dev->fd = open(path, O_RDWR|O_DIRECT);
	if (dev->fd < 0) {
		INFO("Failed to open device %s with write permission, retrying without", path);
		dev->fd = open(path, O_RDONLY|O_DIRECT);
	}
	return dev->fd >= 0;
}

void disk_dev_close(disk_dev_t *dev)
{
	close(dev->fd);
	dev->fd = -1;
}

ssize_t disk_dev_read(disk_dev_t *dev, uint64_t offset_bytes, uint32_t len_bytes, void *buf, io_result_t *io_res)
{
	ssize_t ret = pread(dev->fd, buf, len_bytes, offset_bytes);
	if (ret == len_bytes) {
		io_res->data = DATA_FULL;
		io_res->error = ERROR_NONE;
		return ret;
	} else if (ret > 0) {
		io_res->data = DATA_PARTIAL;
		io_res->error = ERROR_NONE;
		return ret;
	} else if (ret == 0) {
		io_res->data = DATA_NONE;
		io_res->error = ERROR_NONE;
		return ret;
	} else {
		// ret < 0, i.e. error
		INFO("Error reading from disk, offset=%lu len=%u errno=%d (%s)",
			offset_bytes, len_bytes, errno, strerror(errno));
		io_res->data = DATA_NONE;
		io_res->error = ERROR_UNCORRECTED;
		io_res->sense_len = 0;
		memset(&io_res->info, 0, sizeof(io_res->info));
		return -1;
	}

	//TODO: Handle EINTR with a retry
}

ssize_t disk_dev_write(disk_dev_t *dev, uint64_t offset_bytes, uint32_t len_bytes, void *buf, io_result_t *io_res)
{
	ssize_t ret = pwrite(dev->fd, buf, len_bytes, offset_bytes);
	if (ret == len_bytes) {
		io_res->data = DATA_FULL;
		io_res->error = ERROR_NONE;
		return ret;
	} else if (ret > 0) {
		io_res->data = DATA_PARTIAL;
		io_res->error = ERROR_NONE;
		return ret;
	} else if (ret == 0) {
		io_res->data = DATA_NONE;
		io_res->error = ERROR_NONE;
		return ret;
	} else {
		// ret < 0, i.e. error
		io_res->data = DATA_NONE;
		io_res->error = ERROR_UNCORRECTED;
		io_res->sense_len = 0;
		memset(&io_res->info, 0, sizeof(io_res->info));
		return -1;
	}

	//TODO: Handle EINTR with a retry
}

void disk_dev_cdb_in(disk_dev_t *dev, unsigned char *cdb, unsigned cdb_len, unsigned char *buf, unsigned buf_size, unsigned *buf_read, unsigned char *sense, unsigned sense_size, unsigned *sense_read, io_result_t *io_res)
{
	(void)sense_size;
	(void)sense;
	(void)buf_size;
	(void)buf;
	(void)cdb_len;
	(void)cdb;
	(void)dev;

	*sense_read = 0;
	*buf_read = 0;
	memset(&io_res, 0, sizeof(*io_res));
	io_res->data = DATA_NONE;
	io_res->error = ERROR_NONE;
}
