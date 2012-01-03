#define grid_handle void*

/**
 * Load a grid form an nc file
 */
grid_handle grid_load(const char* filename);

unsigned long grid_x(grid_handle handle);
unsigned long grid_y(grid_handle handle);

float grid_get_value(grid_handle handle, unsigned long x, unsigned long y);