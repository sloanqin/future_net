#include "route.h"
#include "lib_io.h"
#include "lib_time.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
    print_time("Begin");
    char *topo[MAX_EDGE_NUM];
    int edge_num;
    char *demand[MAX_DEMAND_NUM];
    int demand_num;

#if defined(WIN32)
	char *topo_file = "G:\\competition\\HUAWEI Code Craft\\semiFinals\\test-case\\caseProblem\\topo.csv";
#else
	char *topo_file = argv[1];
#endif
    edge_num = read_file(topo, MAX_EDGE_NUM, topo_file);
    if (edge_num == 0)
    {
        printf("Please input valid topo file.\n");
        return -1;
    }

#if defined(WIN32)
	char *demand_file = "G:\\competition\\HUAWEI Code Craft\\semiFinals\\test-case\\caseProblem\\demand.csv";
#else
	char *demand_file = argv[2];
#endif
    demand_num = read_file(demand, MAX_DEMAND_NUM, demand_file);
    if (demand_num != MAX_DEMAND_NUM)
    {
        printf("Please input valid demand file.\n");
        return -1;
    }

    search_route(topo, edge_num, demand, demand_num);

#if defined(WIN32)
	char *result_file = "G:\\competition\\HUAWEI Code Craft\\semiFinals\\test-case\\caseProblem\\sample_result.csv";
#else
	char *result_file = argv[3];
#endif
    write_result(result_file);
    release_buff(topo, edge_num);
    release_buff(demand, demand_num);

    print_time("End");

	return 0;
}

