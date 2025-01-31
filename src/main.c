

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL_ttf.h>
#include <string.h>
#include <dirent.h>
#include <SDL_image.h>
#include <time.h>

#define WINDOW_X 800
#define WINDOW_Y 600

void game();

typedef struct {
    int balle_x;
    int balle_y;
    int balle_dx;
    int balle_dy;
    int degats;
    int vitesse;
} balle;

typedef struct {
    int x;
    int y;
    int taille;
    int vitesse;
    int vie;
    double temps;
} joueur;

void afficherTexte(SDL_Renderer* renderer, const char* texte, TTF_Font* font, SDL_Color color, int x, int y) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, texte, color);
    if (!surface) {
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect coord = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &coord);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

}



int** chargeCarteDansTab(char* nom, int taille) {
    char chemin[256];
    snprintf(chemin, sizeof(chemin), "../cartes/%s", nom);
    FILE* fichier = fopen(chemin, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier.\n");
        return NULL;
    }
    int** carte = (int**)malloc(taille * sizeof(int*));
    for (int i = 0; i < taille; i++) {
        carte[i] = (int*)malloc(taille * sizeof(int));
    }
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            fscanf(fichier, "%d", &carte[i][j]);
        }
    }
    fclose(fichier);
    return carte;
}

int** initialisationCarte(int taille){
    int** carte = (int**)malloc(taille * sizeof(int*));
    if (carte == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le tableau.\n");
        return NULL;
    }

    for (int i = 0; i < taille; i++) {
        carte[i] = (int*)malloc(taille * sizeof(int));
        if (carte[i] == NULL) {
            fprintf(stderr, "Erreur d'allocation mémoire pour la ligne %d.\n", i);
            for (int j = 0; j < i; j++) {
                free(carte[j]);
            }
            free(carte);
            return NULL;
        }
    }

    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            if(i < taille /2){
            carte[i][j] = 1;
            }else {
                carte[i][j] = 0;
            }
        }
    }

    return carte;
}

void afficher_ATH(SDL_Renderer *renderer, joueur joueur){
    SDL_Texture* texturejoueur = IMG_LoadTexture(renderer, "../images/joueur.png");
    if (!texturejoueur) {
        fprintf(stderr, "Erreur de chargement des textures : %s\n", IMG_GetError());
    }
    TTF_Font* font = TTF_OpenFont("../font/OpenSans-VariableFont_wdth,wght.ttf", 24);
    SDL_Rect vie = {WINDOW_X/10, WINDOW_Y/10, 75, 20};
    SDL_RenderCopy(renderer, texturejoueur, NULL, &vie);
    SDL_Color blanc = {255, 255, 255, 255};

    char str_vie[10];
    snprintf(str_vie, sizeof(str_vie), "%d", joueur.vie);
    afficherTexte(renderer, str_vie, font, blanc, WINDOW_X / 12, WINDOW_Y / 12);
    SDL_DestroyTexture(texturejoueur);
    TTF_CloseFont(font);
}

void afficherBalle(SDL_Renderer *renderer, balle balle, int taille){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect textureBalle = {balle.balle_x, balle.balle_y, taille, taille};
    SDL_RenderFillRect(renderer, &textureBalle);
}

void affichierCarte(int** carte, int taille, SDL_Renderer *renderer, joueur joueur) {

    SDL_Texture* texturejoueur = IMG_LoadTexture(renderer, "../images/joueur.png");

    SDL_Texture* textureBrique1 = IMG_LoadTexture(renderer, "../images/brique1.png");
    SDL_Texture* textureBrique2 = IMG_LoadTexture(renderer, "../images/brique2.png");
    SDL_Texture* textureBrique3 = IMG_LoadTexture(renderer, "../images/brique3.png");

    if (!textureBrique1 || !textureBrique2 || !textureBrique3 || !texturejoueur) {
        fprintf(stderr, "Erreur de chargement des textures : %s\n", IMG_GetError());
    }
    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            SDL_Rect coord = {j * (WINDOW_X / taille), i * (WINDOW_Y / taille), WINDOW_X / taille, WINDOW_Y / taille};

            if (carte[i][j] == 1) {
                SDL_RenderCopy(renderer, textureBrique1, NULL, &coord); // Texture pour les briques de type 1
            }
            else if (carte[i][j] == 2) {
                SDL_RenderCopy(renderer, textureBrique2, NULL, &coord); // Texture pour les briques de type 2
            }
            else if (carte[i][j] == 3) {
                SDL_RenderCopy(renderer, textureBrique3, NULL, &coord); // Texture pour les briques de type 3
            }
            else if (carte[i][j] == 0) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Couleur pour le fond (si nécessaire)
                SDL_RenderFillRect(renderer, &coord);
            }
        }
    }

    SDL_Rect image_joueur = {joueur.x, joueur.y, 75, 20};
    SDL_RenderCopy(renderer, texturejoueur, NULL, &image_joueur);

    afficher_ATH(renderer, joueur);

    SDL_DestroyTexture(texturejoueur);
    SDL_DestroyTexture(textureBrique1);
    SDL_DestroyTexture(textureBrique2);
    SDL_DestroyTexture(textureBrique3);
}

void libererCarte(int** carte, int taille) {
    for (int i = 0; i < taille; i++) {
        free(carte[i]);
    }
    free(carte);
}

int** ajoutBrique(int** carte, int taille){
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    carte[mouseY/(WINDOW_Y/taille)][mouseX/(WINDOW_X/taille)] = (carte[mouseY/(WINDOW_Y/taille)][mouseX/(WINDOW_X/taille)] + 1) % 4;
    return carte;
}

void saveCarte(int** carte, int taille, char* nom){
    char chemin[256];
    snprintf(chemin, sizeof(chemin), "../cartes/%s.txt", nom);
    FILE* fichier = fopen(chemin, "w");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur d'ouverture du fichier.\n");
    }
    for (int j = 0; j <= taille - 1; j++){
        for (int i = 0; i <= taille - 1; i++) {
            fprintf(fichier, "%d ", carte[j][i]);
        }
        fprintf(fichier, "\n");
    }
    fclose(fichier);
}
char* fen_input(char* nom){
    SDL_Window* window = SDL_CreateWindow(nom, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_X, WINDOW_Y, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Erreur de création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    TTF_Font* font = TTF_OpenFont("../font/OpenSans-VariableFont_wdth,wght.ttf", 24); // Assurez-vous d'avoir ce fichier
    if (!font) {
        fprintf(stderr, "Erreur de chargement de la police: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    SDL_Color noir = {0, 0, 0, 255};
    SDL_Color blanc = {255, 255, 255, 255};

    SDL_Rect zoneSaisie = {200, 250, 400, 50};

    char* texte = (char*)malloc(256);
    if (!texte) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        free(texte);
        return NULL;
    }
    texte[0] = '\0';
    bool saisie = 1;

    SDL_StartTextInput();

    SDL_Event event;
    while (saisie) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                saisie = 0;
            } else if (event.type == SDL_TEXTINPUT) {
                if (strlen(texte) < 256 - 1) {
                    strncat(texte, event.text.text, 256 - strlen(texte) - 1);
                }
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(texte) > 0) {
                    texte[strlen(texte) - 1] = '\0';
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN) {
                    saisie = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &zoneSaisie);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &zoneSaisie);

        afficherTexte(renderer, texte, font, noir, zoneSaisie.x + 10, zoneSaisie.y + 10);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_StopTextInput();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return texte;
}

int fen_QCM(char* texte[], int taille, char* nom) {
    SDL_Window* window = SDL_CreateWindow(nom,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_Y, 400,SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Erreur de création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* font = TTF_OpenFont("../font/OpenSans-VariableFont_wdth,wght.ttf", 24);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
    SDL_RenderClear(renderer);

    for(int i = 0; i < taille; i++){
        SDL_Rect bouton = {(WINDOW_Y / taille) * i, 300, WINDOW_Y / taille, 50};
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &bouton);
        SDL_Color blanc = {255, 255, 255, 255};
        afficherTexte(renderer, texte[i], font, blanc, bouton.x + 30, bouton.y + 10);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &bouton);
    }
    //SDL_Rect boutonOui = {150, 200, 100, 50}; // x, y, largeur, hauteur
    //SDL_Rect boutonNon = {350, 200, 100, 50};

    // Couleur des boutons (bleu)
    //SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Bleu
    //SDL_RenderFillRect(renderer, &boutonOui);
    //SDL_RenderFillRect(renderer, &boutonNon);
    // Texte pour les boutons et la question
    SDL_Color blanc = {255, 255, 255, 255}; // Couleur blanche
    //afficherTexte(renderer, "Oui", font, blanc, boutonOui.x + 30, boutonOui.y + 10);
    //afficherTexte(renderer, "Non", font, blanc, boutonNon.x + 30, boutonNon.y + 10);
    afficherTexte(renderer, nom, font, blanc, 100, 100);

    // Couleur du texte (à simuler ici, SDL ne gère pas directement le texte)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
    //SDL_RenderDrawRect(renderer, &boutonOui); // Bordures blanches
    //SDL_RenderDrawRect(renderer, &boutonNon);

    SDL_Rect bouton = {WINDOW_X/20, WINDOW_Y/30, WINDOW_Y/20, WINDOW_Y/20};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &bouton);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                TTF_CloseFont(font);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                return false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                if (y >= 300 && y <= 350) {
                    TTF_CloseFont(font);
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    return x / (WINDOW_Y / taille);
                } else if (x >= WINDOW_X/20 && x <= WINDOW_X/20 + WINDOW_Y/20 && y >= WINDOW_Y/20 && y <= WINDOW_Y/20 + WINDOW_Y/20){
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    game();
                }
            }
        }
    }
}

void CreatCarte(int taille, SDL_Renderer *renderer, joueur joueur){
    int** carte = initialisationCarte(taille);
    char* nom = (char*)malloc(256);
    int saisie = 1;
    SDL_Event event;
    while (saisie) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                saisie = 0;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    carte = ajoutBrique(carte, taille);
                }
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    saisie = 0;
                    char* ouiNon[] = {"oui", "non"};
                    if (fen_QCM(ouiNon, 2, "sauvegarder ?") == 0){
                        nom = fen_input("sauvegarder ?");
                        saveCarte(carte, taille, nom);
                    }
                }
            }
            affichierCarte(carte, taille, renderer, joueur);
            SDL_RenderPresent(renderer);
        }
        SDL_Delay(200);
    }
    free(nom);
    libererCarte(carte, taille);
}


char* choixCarte(){
    char **cartes = NULL;
    int count = 0;
    const char *dossier = "../../Projet2A/cartes";
    struct dirent *entree;
    DIR *rep = opendir(dossier);

    if (rep == NULL) {
        perror("Erreur lors de l'ouverture du dossier");
    }

    printf("Contenu du dossier '%s':\n", dossier);
    while ((entree = readdir(rep)) != NULL) {
        if (strcmp(entree->d_name, ".") != 0 && strcmp(entree->d_name, "..") != 0) {
            char **temp = realloc(cartes, (count + 1) * sizeof(char *));
            if (temp == NULL) {
                perror("Erreur lors de l'allocation mémoire");
                for (int i = 0; i < count; i++) {
                    free(cartes[i]);
                }
                free(cartes);
                closedir(rep);
                return "";
            }
            cartes = temp;
            cartes[count] = strdup(entree->d_name);
            count++;
        }
    }
    closedir(rep);
    return cartes[fen_QCM(cartes, count, "choix de la map")];
}

void degat_joueur(joueur joueur){
    if (joueur.vie == 1){
        game();
    } else {
        joueur.vie -= 1;
    }
}

balle deplacement(balle balle, int** carte, int taille, joueur joueur){
    if (balle.balle_x == WINDOW_X || balle.balle_x == 0){
        balle.balle_dx *= -1;
    } else if (balle.balle_y == 0) {
        balle.balle_dy *= -1;
    } else if (balle.balle_y > WINDOW_Y - 25 && balle.balle_x > joueur.x && balle.balle_x < joueur.x + joueur.taille) { //colision avec le joueur
        balle.balle_dy *= -1;
    }else if (balle.balle_y >=  WINDOW_Y) {
        balle.balle_x = WINDOW_X /2;
        degat_joueur(joueur);
        balle.balle_y = 20;
    } else if (carte[balle.balle_y / (WINDOW_Y / taille)][balle.balle_x / (WINDOW_X / taille)] > 0){
        if (carte[(balle.balle_y - balle.vitesse) / (WINDOW_Y / taille)][balle.balle_x / (WINDOW_X / taille)] > 0){
            balle.balle_dx *= -1;
            carte[balle.balle_y / (WINDOW_Y / taille)][balle.balle_x / (WINDOW_X / taille)] -= balle.degats;//colision avec une brique
        } else if (carte[balle.balle_y / (WINDOW_Y / taille)][(balle.balle_x - balle.vitesse) / (WINDOW_X / taille)] > 0){
            balle.balle_dy *= -1;
            carte[balle.balle_y / (WINDOW_Y / taille)][balle.balle_x / (WINDOW_X / taille)] -= balle.degats; //colision avec une brique
        }
    }
    balle.balle_x += balle.balle_dx;
    balle.balle_y += balle.balle_dy;
    return balle;
}

void AugJoueur() {

}

void OuvrirParametres(SDL_Renderer *renderer) {
    SDL_Event event;
    bool encours = true;

    TTF_Font* font = TTF_OpenFont("../font/OpenSans-VariableFont_wdth,wght.ttf", 24);
    if (!font) {
        fprintf(stderr, "Erreur de chargement de la police: %s\n", TTF_GetError());
        return;
    }

    while (encours) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
                encours = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                if (y >= 150 && y <= 200) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderClear(renderer);
                    SDL_Color noir = {0, 0, 0, 255};
                    afficherTexte(renderer, "Parametres", font, noir, 300, 100);
                    afficherTexte(renderer, "Degats X2", font, noir, 300, 150);
                    afficherTexte(renderer, "Balle X2", font, noir, 300, 200);
                    afficherTexte(renderer, "Allongement de la barre", font, noir, 300, 250);
                    char message[256];
                    snprintf(message, sizeof(message), "Degats de la balle : %d");
                    afficherTexte(renderer, message, font, noir, 300, 350);
                    SDL_RenderPresent(renderer);
                    SDL_Delay(2000);
                } else if (y >= 200 && y <= 250) {
                    printf("Balle X2 activee !\n");
                } else if (y >= 250 && y <= 300) {

                    printf("Barre allongee !\n");
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_Color noir = {0, 0, 0, 255};
        afficherTexte(renderer, "Degats X2", font, noir, 300, 150);
        afficherTexte(renderer, "Balle X2", font, noir, 300, 200);
        afficherTexte(renderer, "Allongement de la barre", font, noir, 300, 250);
        afficherTexte(renderer, "retour", font, noir, 300, 250);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
}

void game(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Window *window = SDL_CreateWindow("Fenêtre Bleue",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_X, WINDOW_Y,SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Erreur de création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur d'initialisation de SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);

    int taille = 20;
    int nb_balles = 1;
    balle *balles = malloc(nb_balles * sizeof(balle));
    balle balle1;
    balle1.balle_x = WINDOW_X/2;
    balle1.balle_y = 200;
    balle1.vitesse = 1;
    balle1.balle_dx = balle1.vitesse;
    balle1.balle_dy = balle1.vitesse;
    balle1.degats = 1;
    balles[0] = balle1;

    printf("test");
    joueur joueur;
    joueur.x = WINDOW_X/2;
    joueur.y = WINDOW_Y - 30;
    joueur.vie = 1;
    joueur.vitesse = 7;
    joueur.taille = 75;
    char* option[] = {"creer une carte", "charger une carte","Paremetre des bonus"};
    int menuPrincipal = fen_QCM(option, 4, "");
    printf("test");
    if (menuPrincipal == 0){
        CreatCarte(taille, renderer, joueur);
    }
    else if (menuPrincipal == 1){
        int ** carte = chargeCarteDansTab(choixCarte(), 20);

        int saisie = 1;
        SDL_Event event;
        while (saisie) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    saisie = 0;
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        saisie = 0;
                    } else if (event.key.keysym.sym == SDLK_LEFT) {
                        joueur.x -= joueur.vitesse;
                    } else if (event.key.keysym.sym == SDLK_RIGHT) {
                        joueur.x += joueur.vitesse;
                    }
                }
                if (joueur.x < 0) {
                    joueur.x = 0;
                } else if (joueur.x > WINDOW_X - 75) {
                    joueur.x = WINDOW_X - 75;
                }
            }
            affichierCarte(carte, taille, renderer, joueur);
            for(int i = 0; i<nb_balles;i++){
                balles[i] = deplacement(balles[i], carte, taille, joueur);
                afficherBalle(renderer, balles[i], taille);
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(10);
        }
        SDL_Delay(200);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }else if(menuPrincipal == 2) {
        OuvrirParametres(renderer);
    }else if(menuPrincipal == 3);
    {
        int menuAdversaire = fen_QCM(option, 2, "");
        if (menuAdversaire == 0) {

        }
    }
}


int main(int argc, char *argv[]) {
    game();
    return 0;
}

