#include "game.cpp"

// full game with little optimization + spherical mirrors + cubical mirrors
int main() {

    int noLineMirrors, noCircleMirrors, noSquareMirrors;
    printf("Enter number of line mirrors (<= 10): ");
    cin >> noLineMirrors;
    while (noLineMirrors > 10 || noLineMirrors < 0) {
        printf("Enter number of line mirrors (<= 10): ");
        cin >> noLineMirrors;
    }
    printf("Enter number of circle mirrors (<= 10): ");
    cin >> noCircleMirrors;
    while (noCircleMirrors > 10 || noCircleMirrors < 0) {
        printf("Enter number of circle mirrors (<= 10): ");
        cin >> noCircleMirrors;
    }
    printf("Enter number of square mirrors (<= 10): ");
    cin >> noSquareMirrors;
    while (noSquareMirrors > 10 || noSquareMirrors < 0) {
        printf("Enter number of square mirrors (<= 10): ");
        cin >> noSquareMirrors;
    }

    initCanvas("Archer", 1500, 700);

    Game(noLineMirrors, noCircleMirrors, noSquareMirrors);
}
