#ifndef LIST_UNIT_TESTS_H_
#define LIST_UNIT_TESTS_H_

void TestFirstInsert();
void TestMultipleInsertionsAndDeleteMiddle();
void TestDeleteHead();
void TestDeleteTail();
void TestDeleteLastRemainingElement();
void TestReallocation();
void TestReuseAfterDelete();
void TestDumpWithBeautifulErrorPrevVisualization();
void TestDumpWithBeautifulErrorNextVisualization();
void TestDumpWithCycleInList();
void TestLinearize();
void TestReallocDown();
void TestListReallocDownAsPossibleAsYouCan();
void TestListShrinkToFit();

void AllTests();


#endif // LIST_UNIT_TESTS_H_
