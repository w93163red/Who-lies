#include <iostream>
#include <string>
#include <stdio.h>
#include <algorithm>
#include <string.h>

using namespace std;

int main()
{
	char* a = "dfsde";
	char buf[512];

	sprintf(buf, "%s %s", "ADD ",a);
	printf("%s\n", buf);		

	
	return 0;

}
