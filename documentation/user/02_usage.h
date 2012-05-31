/**
 * @page usage Using ASAGI
 *
 * @section minexamples Minimal examples
 *
 * These are minimal C, C++ and Fortran examples that load a 2-dimensional grid
 * and print the value at (0,0). In each case the grid contains floating point
 * values.
 * 
 * C example:
 * @include minimal.c
 * 
 * C++ example:
 * @include minimal.cpp
 * 
 * Fortran example:
 * @include minimal.f90
 *
 * @section Dimensions
 *
 * ASAGI supports grids with up to three dimensions. The number of dimension
 * cannot be specified by calling an ASAGI function but depends on the NetCDF
 * input file. For example, to access an integer of a 2-dimensional grid in C++,
 * use <code>getInt2D</code>. For a 3-dimensional grid, the corresponding call
 * is <code>getInt3D</code>.
 * 
 * @section lod Level of detail
 *
 * A grid can have multiple resolutions. Each resolution is identified by a
 * level id (level of detail). If the number of levels is not specified when
 * creating a grid, the grid will contain only one level of detail. In this case
 * you can also omit the level id in all other functions, since level 0 will be
 * used by default. (C does not support default arguments or overloading,
 * therefore omitting arguments is not possible when using the C interface.)
 *
 * For grids with multiple levels {@link Grid::open()} must be called once for
 * each level. Several levels can be stored in a single NetCDF file with
 * different variable names. (Use {@link Grid::setParam()} to specify the
 * variable name.) The coarsest resolution should have the level id 0. With
 * ascending level id, the resolution should get finer. When accessing values
 * with any <code>get</code> function, the level of detail can be selected with
 * the last argument. The function {@link Grid::close()} has to be called only
 * once for the whole grid.
 *
 * @section coordinatemapping Coordinate mapping
 *
 * ASAGI distinguishes between actual coordinates and internal array indexes.
 * All functions, that return a grid value, expect actual coordinates. ASAGI
 * maps each coordinate to an array index using the coordinate variables from
 * the NetCDF file (see section @ref netcdffiles on how specify coordinate
 * variables in NetCDF files). If no coordinate variable is available, the
 * mapping is omitted. After the mapping, the coordinate is rounded to the
 * nearest array index. ASAGI does not interpolate between array values.
 *
 * The actual range of the grid can be obtained with the
 * <code>getMin</code>/<code>getMax</code> functions. They also return
 * coordinates, not array indexes. It is erroneous to access values outside
 * range of the grid.
 *
 * The range of a dimension can be @f$(-\infty,\infty)@f$. This is the case if
 * the size of the dimension in the NetCDF file is one.
 *
 * @section valuepos Value position
 *
 * ASAGI supports cell-centered and vertex-centered grids. The value position
 * can be switched with {@link Grid::setParam()}.
 *
 * @image html valueposition.svg "Cell-centered and vertex-centered grids"
 * @image latex valueposition.eps "Cell-centered and vertex-centered grids"
 * 
 * @section netcdffiles NetCDF files
 * All NetCDF files opened with ASAGI should respect the COARDS conventions
 * (http://ferret.wrc.noaa.gov/noaa_coop/coop_cdf_profile.html). However, ASAGI
 * has some further limitations:
 * @li The attributes <code>scale_factor</code> and <code>add_offset</code> are
 *  ignored. Besides conversion between data types, ASAGI does not modify the
 *  values.
 * @li Since ASAGI does not change the NetCDF file, all values have to be
 *  present in the file. Attributes, like <code>_FillValue</code> and
 *  <code>missing_value</code>, are not supported.
 * @li ASAGI is not aware of any units. It is up to the user of the library to
 *  interpret the values correctly.
 * @li Variables with more than three dimensions are not supported.
 *
 * It is possible to open a NetCDF file by different grids or levels at the same
 * time. This allows you, for example, to store all levels of one grid in a
 * single NetCDF file. In this case the levels must be distinguished by the
 * variable names.
 *
 * @section multithread Multi-thread support
 *
 * When compiled with <code>THREADSAFTY=ON</code> (see section @ref Compilation)
 * all functions are thread-safe. However, there are some restrictions due to
 * MPI implementations. To receive values from a grid with different threads MPI
 * must support at least <code>MPI_THREAD_SERIALIZED</code>. If you want to
 * <code>open</code> or <code>close</code> several grids at the same time,
 * support for <code>MPI_THREAD_MULTIPLE</code> is required.
 */