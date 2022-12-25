int f(int a,int b,int c) {
	return a + b + c;
}

int main(){
	int a = 1;
	int b = 2;
	a = f(a + b, a - b, a + 3 * b);
	return 0;
}
