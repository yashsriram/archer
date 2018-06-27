#include "lightplay.cpp"

//******high processing demands
//******can do better by less checking every time
//******and more intelligent checking near the mirror
//******reflection should be more at the mirror
//******limit on noSquareMirrors
//******sometimes dosent start
// full game with little optimization + spherical mirrors + cubical mirrors
//******doesn't handle reflection on both at same time ie corners
int main() {

    int noLineMirrors, noCircleMirrors, noSquareMirrors;
    printf("Enter number of Line mirrors\n");
    cin >> noLineMirrors;
    printf("Enter number of Circle mirrors\n");
    cin >> noCircleMirrors;
    printf("Enter number of Square mirrors\n");
    cin >> noSquareMirrors;

    initCanvas("LightPlay", 1500, 700);

    Rectangle border(750, 350, 1500, 700);
    border.setColor(COLOR(65, 65, 65)).setFill();
    border.imprint();
    border.reset(750, 350, 1400, 600);
    border.setColor(COLOR(0, 0, 0)).setFill();
    border.imprint();

//define a light class rotate ,reflect until last non reflecting surface.
    LightPlay lightPlay = LightPlay(noLineMirrors, noCircleMirrors, noSquareMirrors);
}
