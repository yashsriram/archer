#include "game.cpp"

// full game with little optimization + spherical mirrors + cubical mirrors
int main() {

    int noLineMirrors, noCircleMirrors, noSquareMirrors;
    printf("Enter number of Line mirrors (<= 10)\n");
    cin >> noLineMirrors;
    while (noLineMirrors > 10 || noLineMirrors < 0) {
        printf("Enter number of Line mirrors (<= 10)\n");
        cin >> noLineMirrors;
    }
    printf("Enter number of Circle mirrors (<= 10)\n");
    cin >> noCircleMirrors;
    while (noCircleMirrors > 10 || noCircleMirrors < 0) {
        printf("Enter number of Circle mirrors (<= 10)\n");
        cin >> noCircleMirrors;
    }
    printf("Enter number of Square mirrors (<= 10)\n");
    cin >> noSquareMirrors;
    while (noSquareMirrors > 10 || noSquareMirrors < 0) {
        printf("Enter number of Square mirrors (<= 10)\n");
        cin >> noSquareMirrors;
    }

    initCanvas("Archer", 1500, 700);

    Game(noLineMirrors, noCircleMirrors, noSquareMirrors);
}
