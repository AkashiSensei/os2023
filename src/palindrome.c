#include <stdio.h>
#include <string.h>
int main() {
	int n;
	scanf("%d", &n);

	char str[20] = {'\0'};
	sprintf(str, "%d", n);
	//printf("%s\n", str);
	int len = strlen(str);
	//printf("%d\n", len);
	int is = 1;
	for(int i = 0; i < len / 2; i++) {
		if(str[i] != str[len - 1 - i]) {
			is = 0;
			break;
		}
	}

	if (is) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
