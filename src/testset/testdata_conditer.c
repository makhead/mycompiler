int main(){
	int i = 0;
	int j = 0;
	int sum = 0;
	while(i <= 100) {
		i = i + 1;
		sum = sum + i;
	}
	if(sum > 100) {
		sum = sum / 10;
	}
	else {
		sum = sum * 10;
	}

	return 0;
}
