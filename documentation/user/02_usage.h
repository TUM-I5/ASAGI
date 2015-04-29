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
 * @section gridtypes Grid types
 *
 * ASAGI distinguishes between three different grid types:
 * @li <b>FULL</b> The whole grid will be loaded during the initialization. The
 *  file is not accessed during runtime. (default)
 * @li <b>CACHE</b> ASAGI is used as a cache. After initialization, the cache
 *  will be empty. Each access to an element, will put the corresponding block
 *  into the cache for later usage.
 * @li <b>PASS-THROUGH</b> ASAGI will pass each access to the underlying file
 *  system without any caching, etc.
 *
 * Full storage does not automatically mean, that the full grid is stored on
 * every CPU. If {@link asagi::Grid::setComm()} and/or
 * {@link asagi::Grid::setThreads()} are called, the initial grid will be
 * distributed among all nodes resp. CPUs.
 * If the cache-grid is used and {@link asagi::Grid::setThreads()} and/or
 * {@link asagi::Grid::setComm()} are set, ASAGI will copy the data from
 * other NUMA domains and/or other MPI processes. Only if it is not available
 * in another cache, the data will be fetched from the file.
 *
 * @section Dimensions
 *
 * ASAGI supports grids with up to <code>MAX_DIMENSIONS</code> dimensions.
 * (<code>MAX_DIMENSIONS</code> is 4 by default, but can be changed during
 * compilation of ASAGI.) The number of actual dimensions in the grid
 * cannot be specified by calling an ASAGI function but depends on the netCDF
 * input file.
 * 
 * @remark The order in the dimension in the netCDF file is in Fortran style
 * (column-major, see @ref netcdffiles) but the ASAGI interface uses C/C++
 * ordering (row-major).
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
 * For grids with multiple levels {@link asagi::Grid::open()} must be called once
 * for each level. Several levels can be stored in a single NetCDF file with
 * different variable names. (Use {@link asagi::Grid::setParam()} to specify the
 * variable name.) The coarsest resolution should have the level id 0. With
 * ascending level id, the resolution should get finer. When accessing values
 * with any <code>get</code> function, the level of detail can be selected with
 * the last argument. The function {@link asagi::Grid::close()} has to be called
 * only once for the whole grid.
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
 * The actual range of the grid can be obtained with
 * {@link asagi::Grid::getMin()}/{@link asagi::Grid::getMax()}. They also return
 * coordinates, not array indexes. It is erroneous to access values outside
 * range of the grid.
 *
 * The range of a dimension can be @f$(-\infty,\infty)@f$. This is the case if
 * the size of the dimension in the netCDF file is one.
 *
 * @section valuepos Value position
 *
 * ASAGI supports cell-centered and vertex-centered grids. The value position
 * can be switched with {@link asagi::Grid::setParam()}.
 *
 * @image html valueposition.svg "Cell-centered and vertex-centered grids"
 * @image latex valueposition.eps "Cell-centered and vertex-centered grids"
 * 
 * @section netcdffiles NetCDF files
 *
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
 * When compiled with <code>THREADSAFE=ON</code> (see section @ref Compilation)
 * all functions are thread-safe. However, there are some restrictions due to
 * MPI implementations. If your MPI library is not thread-safe, you have to add
 * the additional flag <code>THREADSAFE_MPI=ON</code> which will mare sure that
 * ASAGI does not call MPI functions from different threads at the same time.
 * However, in this case, you are not allowed to call MPI <b>and</b> ASAGI
 * functions at the same time.
 *
 * Multi-thread support is required if you want to use ASAGI's NUMA
 * functionality (see @ref NUMA).
 *
 * @section NUMA
 *
 * ASAGI is able to detect the NUMA domains of your node. If more than one NUMA
 * domain is detected, ASAGI will place a cache on each NUMA domain to increase
 * locality. To enable the NUMA detection, call
 * {@link asagi::Grid::setThreads()} with the <b>total</b> number of threads
 * you are using. In this case, {@link asagi::Grid::open()} has to be called by
 * all threads and is a collective operation.
 *
 * @section Parameters
 *
 * ASAGI supports several parameters for each grid:
 *
 * <table>
 *  <tr>
 *   <th>Name</th>
 *   <th>Values</th>
 *   <th>Description</th>
 *   <th>Grid-global @ref gridglobal "(*)"</th>
 *  </tr>
 *  <tr>
 *   <td>GRID</td>
 *   <td>FULL | CACHE | PASS-THROUGH</td>
 *   <td>The grid type (see @ref gridtypes)</td>
 *   <td>yes</td>
 *  </tr>
 *  <tr>
 *   <td>NUMA-CACHE</td>
 *   <td>YES | NO</td>
 *   <td>For full-grids, try local caches before using MPI</td>
 *   <td>yes</td>
 *  </tr>
 *  <tr>
 *   <td>VALUE-POSITION</td>
 *   <td>CELL-CENTERED | VERTEX-CENTERED</td>
 *   <td>The value position (see @ref valuepos)</td>
 *   <td>yes</td>
 *  </tr>
 *  <tr>
 *   <td>TIME-DIMENSION</td>
 *   <td>int</td>
 *   <td>The dimension that holds the time (default is -1 which means no time
 *    dimension exists). ASAGI treats time dimension specially.</td>
 *   <td>yes</td>
 *  </tr>
 *  <tr>
 *   <td>VARIABLE</td>
 *   <td>string</td>
 *   <td>The variable name in the netCDf file. (default: z)</td>
 *   <td>no</td>
 *  </tr>
 *  <tr>
 *   <td>BLOCK-SIZE-<i>X</i></td>
 *   <td>int</td>
 *   <td>The size of a block in dimension <i>X</i>.</td>
 *   <td>no</td>
 *  </tr>
 *  <tr>
 *   <td>CACHE-SIZE</td>
 *   <td>int</td>
 *   <td>The size of the cache (in blocks) on each CPU.</td>
 *   <td>no</td>
 *  </tr>
 *  <tr>
 *   <td>CACHE-HAND-SPREAD</td>
 *   <td>int</td>
 *   <td>ASAGI uses the clock algorithm to approx. LRU. This parameter
 *    specifies the difference of the 2 hands in the clock. Lower values
 *    result in a faster algorithm but a worse approximation.</td>
 *   <td>no</td>
 *  </tr>
 * </table>
 *
 * @anchor gridglobal (*) If yes, the parameter can only be set for all levels
 *  at the same time. Set the parameter <code>level</code> in
 *  {@link asagi::Grid::setParam()} to 0 to change value.
 *
 * @section accesscounter Access counters
 *
 * ASAGI supports several access counters to measure the throughput of the
 * library and get information about effectiveness of the caches:
 *
 * <table>
 *  <tr>
 *   <th>Name</th>
 *   <th>Description</th>
 *  </tr>
 *  <tr>
 *   <td>accesses</td>
 *   <td>Total number of data accesses</td>
 *  </tr>
 *  <tr>
 *    <td>numa_transfers</td>
 *    <td>Number of blocks transfered between CPUs</td>
 *  <tr>
 *   <td>mpi_transfers</td>
 *   <td>Number of blocks transfered between processes</td>
 *  </tr>
 *  <tr>
 *   <td>file_load</td>
 *   <td>Number of blocks loaded from file (after initialization)</td>
 *  </tr>
 *  <tr>
 *   <td>local_hits</td>
 *   <td>Number values that where already in local NUMA domain</td>
 *  </tr>
 *  <tr>
 *   <td>node_hits</td>
 *   <td>Number values that where already on the local node</td>
 *  </tr>
 *  <tr>
 *   <td>local_misses</td>
 *   <td>Number of values that where not already in local memory</td>
 *  </tr>
 * </table>
 *
 * @remark If more than one thread is used and ASAGI is not compiled with
 *  <code>THREADSAFE_COUNTER=ON</code>, the counters might be inaccurate.
 */
