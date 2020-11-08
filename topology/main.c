//Currently Windows supports only 2 logical processors per core


#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include "Topology.h"

int main(int argc, char* argv[]) {
	if (argc > 2) {
		printf("Too many arguments supplied\n");
		return 1;
	}
	else if (argc == 1) {
		printf("Atleast one argument expected\n");
		return 1;
	}
	if ((strcmp(argv[1], "ProcessorTopology")) && (strcmp(argv[1], "NumaTopology")) && (strcmp(argv[1], "GroupTopology"))) {
		printf("Invalid argument\n");
		return 1;
	}

	if (!(strcmp(argv[1], "ProcessorTopology"))) {
		DWORD CoreReturnLength = 0;
		LOGICAL_PROCESSOR_RELATIONSHIP Core = RelationProcessorCore;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX CoreBuffer = CreateGLPIXStruct(Core, &CoreReturnLength);
		if (NULL == CoreBuffer) {
			return 1; //printing error message in function call so not printing here
		}
		DWORD res = ProcessorTopology(Core, CoreBuffer, CoreReturnLength);
		if (res != 0) {
			printf("Error : %lu", res);
			return 1;
		}
	}
	else if (!(strcmp(argv[1], "NumaTopology"))) {
		DWORD NumaReturnLength = 0;
		LOGICAL_PROCESSOR_RELATIONSHIP Numa = RelationNumaNode;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX NumaBuffer = CreateGLPIXStruct(Numa, &NumaReturnLength);
		if (NULL == NumaBuffer) {
			return 1;
		}
		NumaTopology(Numa, NumaBuffer, NumaReturnLength);
	}
	else if (!(strcmp(argv[1], "GroupTopology"))) {
		DWORD GroupReturnLength = 0;
		LOGICAL_PROCESSOR_RELATIONSHIP Group = RelationGroup;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX GroupBuffer = CreateGLPIXStruct(Group, &GroupReturnLength);
		if (NULL == GroupBuffer) {
			return 1;
		}
		GroupTopology(Group, GroupBuffer, GroupReturnLength);
	}
}