int main(void) {

    int i, A[20], C[20], D[20];

    for(i = 0; i < 20; i++) {
        A[i] = C[i];
        D[i] = A[3*i - 4];
        D[i-1] = C[2*i];
    }

    return 0;
}