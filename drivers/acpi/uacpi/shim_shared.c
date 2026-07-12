#include <uacpi/internal/context.h>

bool shim_should_log(uacpi_log_level level);

bool shim_should_log(uacpi_log_level level)
{
	return uacpi_should_log(level);
}
