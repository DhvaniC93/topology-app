#include <stdio.h>
#include <windows.h>
#include <winnt.h>
#include "Topology.h"


PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX CreateGLPIXStruct(LOGICAL_PROCESSOR_RELATIONSHIP relation, PDWORD returnLength) {
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer = NULL;

	BOOL strct = GetLogicalProcessorInformationEx(relation, buffer, returnLength);

	if (FALSE == strct) {
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			if (buffer) {
				free(buffer);
			}

			buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)malloc(*returnLength);

			if (NULL == buffer) {
				printf("\nError: Allocation failure\n");
				return NULL;
			}
		}
		else {
			printf("\nError %d\n", GetLastError());
			return NULL;
		}
	}

	strct = GetLogicalProcessorInformationEx(relation, buffer, returnLength);
	if (NULL == buffer) {
		printf("buffer is NULL\n");
		return NULL;
	}

	return buffer;
}

DWORD ProcessorTopology(LOGICAL_PROCESSOR_RELATIONSHIP rel, PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ptr, DWORD returnLength) {
	printf("\n***********************PROCESSOR TOPOLOGY***********************\n\n");
	printf("----------------------------------------------------------------\n");
	printf("|  Core  |  Logical Processors  |  Group Number  |  Numa Node  |\n");
	printf("----------------------------------------------------------------\n");

	DWORD byteOffset = 0;
	DWORD CoreCount = 0;
	DWORD LogicalProcessorCount = 0;
	PUSHORT NodeNumber;
	PULONG index;

	while (byteOffset < returnLength) {
		//no need to check flags right?
			//printing core count
		if (CoreCount < 10) {
			printf("|   0%lu   |       ", CoreCount);
		}
		else if (CoreCount < 100) {
			printf("|   %lu   |       ", CoreCount);
		}
		else {
			printf("|  %lu   |       ", CoreCount);
		}
		CoreCount++;

		//printing number of LPs
		DWORD numBitsSet = CountSetBits(ptr->Processor.GroupMask->Mask);
		for (DWORD j = 0; j < numBitsSet; j++) {

			if (LogicalProcessorCount < 10) {
				printf(" 0%lu ", LogicalProcessorCount);
			}
			else if (LogicalProcessorCount < 100) {
				printf(" %lu ", LogicalProcessorCount);
			}
			else {
				printf(" %lu ", LogicalProcessorCount);
			}
			if (j < (numBitsSet - 1)) {
				printf(",");
			}
			LogicalProcessorCount++;
		}
		if (LogicalProcessorCount <= 100) {
			printf("      |");
		}
		else {
			printf("    |");
		}

		//printing group number
		printf("       %hu        |", ptr->Processor.GroupMask->Group);

		//printing numa node number 
		_BitScanForward(&index, (unsigned __int64)(ptr->Processor.GroupMask->Mask));
		PROCESSOR_NUMBER procnum;
		procnum.Group = ptr->Processor.GroupMask->Group;
		procnum.Number = (BYTE)index;
		procnum.Reserved = 0;
		BOOL fl = GetNumaProcessorNodeEx(&procnum, &NodeNumber);
		if (fl == 0) {
			return GetLastError(); //better error handling?
		}
		else {
			printf("       %hu     |\n", NodeNumber);
		}


		byteOffset += ptr->Size;
		(char*)ptr += ptr->Size;
	}
	printf("----------------------------------------------------------------\n");
	return 0;
}

DWORD NumaTopology(LOGICAL_PROCESSOR_RELATIONSHIP rel, PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ptr, DWORD returnLength) {
	printf("\n*****************************************NUMA TOPOLOGY***********************************************\n\n");
	printf("-------------------------------------------------------------------------------------------------------\n");
	printf("|  Numa Node  |  Number of LPs  |  Group Number  |     Processor Mask     |  Available Memory (bytes) |\n");
	printf("-------------------------------------------------------------------------------------------------------\n");

	DWORD byteOffset = 0;
	DWORD NumaNodeCount = 0;

	while (byteOffset < returnLength) {
		//printing numa node number
		printf("|      %lu      |", ptr->NumaNode.NodeNumber);

		//printing number of LPs
		DWORD numLPs = CountSetBits(ptr->NumaNode.GroupMask.Mask);
		if (numLPs < 10) {
			printf("        0%lu       |", numLPs);
		}
		else {
			printf("        %lu       |", numLPs);
		}

		//printing group number
		printf("       %lu        |", ptr->NumaNode.GroupMask.Group);

		//printing processor mask
		printf("       0X%08x       |", ptr->NumaNode.GroupMask.Mask);

		//printing available memory
		PULONGLONG availableMemory;
		GetNumaAvailableMemoryNode((unsigned char)NumaNodeCount, &availableMemory);
		printf("       %llu     \n", availableMemory);


		byteOffset += ptr->Size;
		(char*)ptr += ptr->Size;
		NumaNodeCount++;
	}
	printf("-------------------------------------------------------------------------------------------------------\n");
	return NumaNodeCount;
}

void GroupTopology(LOGICAL_PROCESSOR_RELATIONSHIP rel, PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ptr, DWORD returnLength) {
	printf("\n**************GROUP TOPOLOGY**************\n\n");

	//printing maximum and active group count
	WORD ActiveGroupCount = ptr->Group.ActiveGroupCount;
	printf("Maximum Group count : %hu\n", ptr->Group.MaximumGroupCount);
	printf("Active Group count  : %hu\n\n", ActiveGroupCount);

	//printing maximum and active group count and active processor mask
	for (WORD i = 0; i < ActiveGroupCount; i++) {
		printf("Group                   : %hu\n", i);
		printf("Maximum Processor count : %u\n", ptr->Group.GroupInfo[i].MaximumProcessorCount);
		printf("Active Processor count  : %u\n", ptr->Group.GroupInfo[i].ActiveProcessorCount);
		printf("Processor Mask          : 0X%08x\n\n", ptr->Group.GroupInfo[i].ActiveProcessorMask);
	}

	printf("******************************************\n\n");
}

//function to count number of set bits in mask
DWORD CountSetBits(ULONG_PTR bitMask) {
	DWORD bitSetCount = 0;
	ULONG_PTR bitTest = 1;
	DWORD i;

	for (i = 0; i < (sizeof(ULONG_PTR) * 8); i++) {
		bitSetCount += ((bitMask & bitTest) ? 1 : 0);
		bitTest *= 2;
	}
	return bitSetCount;
}

