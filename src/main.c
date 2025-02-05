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

void menuPrincipal();

typedef struct {
    char *nom;
    int bonus_x;
    int bonus_y;
    int bonus_dx;
    int bonus_dy;
    int degats;
    int vitesse_joueur;
    int vitesse_balle;
    int vie_joueur;
    int score;
    char* skin;
} bonus;


typedef struct {
    int balle_x;
    int balle_y;
    int balle_dx;
    int balle_dy;
    int degats;
    int vitesse;
    char* skin;
} balle;

typedef struct {
    int x;
    int y;
    int taille;
    int vitesse;
    int vie;
    double temps;
    int score;
    char* skin;
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
                carte[i][j] = 0;
            }else {
                carte[i][j] = 0;
            }
        }
    }

    return carte;
}

void afficherBonus(SDL_Renderer *renderer, bonus* tabonus, int nbonus, int taille){
    char chemin[255];
    for (int i = 0; i < nbonus; i++){
        snprintf(chemin, sizeof(chemin), "../images/%s", tabonus[i].skin);
        SDL_Texture* texturebonus = IMG_LoadTexture(renderer, chemin);
        if (!texturebonus) {
            fprintf(stderr, "Erreur de chargement des textures : %s\n", IMG_GetError());
        }
        SDL_Rect bonus = {tabonus[i].bonus_x, tabonus[i].bonus_y, taille, taille};
        SDL_RenderCopy(renderer, texturebonus, NULL, &bonus);
    }
}

void afficher_fond(SDL_Renderer *renderer, joueur joueur){
    SDL_RenderClear(renderer);

    char chemin[255];
    snprintf(chemin, sizeof(chemin), "../images/%s", joueur.skin);
    SDL_Texture* texturejoueur = IMG_LoadTexture(renderer, chemin);

    SDL_Texture* texturefond = IMG_LoadTexture(renderer, "../images/fond.png");
    SDL_Texture* texturescore = IMG_LoadTexture(renderer, "../images/brique1.png");

    if (!texturejoueur || !texturefond || !texturescore) {
        fprintf(stderr, "Erreur de chargement des textures : %s\n", IMG_GetError());
    }
    TTF_Font* font = TTF_OpenFont("../font/OpenSans-VariableFont_wdth,wght.ttf", 24);


    SDL_Rect fond = {0, 0, WINDOW_X, WINDOW_Y};
    SDL_RenderCopy(renderer, texturefond, NULL, &fond);


    SDL_Rect vie = {WINDOW_X*1.05, WINDOW_Y/10, 75, 20};
    SDL_RenderCopy(renderer, texturejoueur, NULL, &vie);

    SDL_Color blanc = {255, 255, 255, 255};
    char str_vie[10];
    snprintf(str_vie, sizeof(str_vie), "%d", joueur.vie);
    afficherTexte(renderer, str_vie, font, blanc, WINDOW_X*1.025, WINDOW_Y / 10);


    SDL_Rect score = {WINDOW_X*1.1, WINDOW_Y/6, 40, 20};
    SDL_RenderCopy(renderer, texturescore, NULL, &score);

    char str_score[10];
    snprintf(str_score, sizeof(str_score), "%03d", joueur.score);
    afficherTexte(renderer, str_score, font, blanc, WINDOW_X*1.025, WINDOW_Y / 6-5);


    SDL_DestroyTexture(texturejoueur);
    SDL_DestroyTexture(texturefond);
    SDL_DestroyTexture(texturescore);
    TTF_CloseFont(font);
}

void afficherBalle(SDL_Renderer *renderer, balle balle, int taille){
    char chemin[255];
    snprintf(chemin, sizeof(chemin), "../images/%s", balle.skin);
    SDL_Texture* textureballe = IMG_LoadTexture(renderer, chemin);
    if (!textureballe) {
        fprintf(stderr, "Erreur de chargement des textures : %s\n", IMG_GetError());
    }
    SDL_Rect image_balle = {balle.balle_x, balle.balle_y, taille, taille};
    SDL_RenderCopy(renderer, textureballe, NULL, &image_balle);
    SDL_DestroyTexture(textureballe);
}

void afficherCarte(int** carte, int taille, SDL_Renderer *renderer, joueur joueur) {
    char chemin[255];
    snprintf(chemin, sizeof(chemin), "../images/%s", joueur.skin);
    SDL_Texture* texturejoueur = IMG_LoadTexture(renderer, chemin);

    SDL_Texture* textureBrique1 = IMG_LoadTexture(renderer, "../images/brique1.png");
    SDL_Texture* textureBrique2 = IMG_LoadTexture(renderer, "../images/brique2.png");
    SDL_Texture* textureBrique3 = IMG_LoadTexture(renderer, "../images/brique3.png");
    SDL_Texture* textureBrique4 = IMG_LoadTexture(renderer, "../images/brique4.png");
    SDL_Texture* textureBriqueTransparente = IMG_LoadTexture(renderer, "../images/brique_transparente.png"); // Texture transparente

    if (!textureBrique1 || !textureBrique2 || !textureBrique3 || !texturejoueur || !textureBriqueTransparente) {
        fprintf(stderr, "Erreur de chargement des textures : %s\n", IMG_GetError());
    }

    for (int i = 0; i < taille; i++) {
        for (int j = 0; j < taille; j++) {
            SDL_Rect coord = {j * (WINDOW_X / taille), i * (WINDOW_Y / taille), WINDOW_X / taille, WINDOW_Y / taille};
            if (carte[i][j] == 1) {
                SDL_RenderCopy(renderer, textureBrique1, NULL, &coord); // Texture pour les briques de type 1
            } else if (carte[i][j] == 2) {
                SDL_RenderCopy(renderer, textureBrique2, NULL, &coord); // Texture pour les briques de type 2
            } else if (carte[i][j] == 3) {
                SDL_RenderCopy(renderer, textureBrique3, NULL, &coord); // Texture pour les briques de type 3
            }else if (carte[i][j] == 4) {
                SDL_RenderCopy(renderer, textureBrique4, NULL, &coord); // Texture pour les briques de type 4
            } else {
                // Afficher une brique transparente
                SDL_RenderCopy(renderer, textureBriqueTransparente, NULL, &coord);
            }
        }
    }

    SDL_Rect image_joueur = {joueur.x, joueur.y, 75, 20};
    SDL_RenderCopy(renderer, texturejoueur, NULL, &image_joueur);

    SDL_DestroyTexture(texturejoueur);
    SDL_DestroyTexture(textureBrique1);
    SDL_DestroyTexture(textureBrique2);
    SDL_DestroyTexture(textureBrique3);
    SDL_DestroyTexture(textureBrique4);
    SDL_DestroyTexture(textureBriqueTransparente);
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
    carte[mouseY/(WINDOW_Y/taille)][mouseX/(WINDOW_X/taille)] = (carte[mouseY/(WINDOW_Y/taille)][mouseX/(WINDOW_X/taille)] + 1) % 5;
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
                    menuPrincipal();
                }
            }
        }
    }
}


char* fen_QCM2(char* texte[], int taille, char* nom, int largeur_image, int longueur_image) {
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
    char chemin[255];
    for(int i = 0; i < taille; i++){
        snprintf(chemin, sizeof(chemin), "../images/%s", texte[i]);
        SDL_Texture* texture = IMG_LoadTexture(renderer, chemin);
        if (!texture) {
            fprintf(stderr, "Erreur de chargement des textures : %s\n", IMG_GetError());
        }
        SDL_Rect image = {(WINDOW_Y / taille) * i, 300, largeur_image, longueur_image};
        SDL_RenderCopy(renderer, texture, NULL, &image);
        SDL_DestroyTexture(texture);
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
                    return texte[(x / (WINDOW_Y / taille))];
                } else if (x >= WINDOW_X/20 && x <= WINDOW_X/20 + WINDOW_Y/20 && y >= WINDOW_Y/20 && y <= WINDOW_Y/20 + WINDOW_Y/20){
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
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
            afficherCarte(carte, taille, renderer, joueur);
            SDL_RenderPresent(renderer);
        }
        SDL_Delay(200);
    }
    free(nom);
    libererCarte(carte, taille);
}

int ** CreatCarteAdversaire(int taille, SDL_Renderer *renderer, joueur joueur) {
    int** carte = initialisationCarte(taille);
    if (carte == NULL) {
        fprintf(stderr, "Erreur : impossible d'initialiser la carte.\n");
        exit(EXIT_FAILURE);
    }

    int saisie = 1;
    SDL_Event event;
    while (saisie) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                saisie = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    carte = ajoutBrique(carte, taille);
                }
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    saisie = 0;
                    SDL_RenderPresent(renderer);
                    return carte;
                }
            }
        }
        afficherCarte(carte, taille, renderer, joueur);
        SDL_RenderPresent(renderer);
        SDL_Delay(200);
    }
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

void degat_joueur(joueur *joueur){
    joueur->vie -= 1;
}


bonus creer_bonus(int x, int y) {
    // Liste des bonus possibles
    bonus bonus_list[] = {
            {
                    .nom = "Bonus Vie",
                    .degats = 0,
                    .skin = "bonus_vie.png",
                    .bonus_x = x,
                    .bonus_y = y,
                    .bonus_dx = 0,
                    .bonus_dy = 4,
                    .vie_joueur = 1,       // Ajoute 1 vie au joueur
                    .vitesse_joueur = 0,
                    .vitesse_balle = 0,
                    .score = 0
            },
            {
                    .nom = "Bonus Vitesse Balle",
                    .degats = 0,
                    .skin = "bonus_vitesse_balle.png",
                    .bonus_x = x,
                    .bonus_y = y,
                    .bonus_dx = 0,
                    .bonus_dy = 2,
                    .vie_joueur = 0,
                    .vitesse_joueur = 0,
                    .vitesse_balle = 2,    // Augmente la vitesse de la balle
                    .score = 0
            },
            {
                    .nom = "Bonus Vitesse Joueur",
                    .degats = 0,
                    .skin = "bonus_vitesse_joueur.png",
                    .bonus_x = x,
                    .bonus_y = y,
                    .bonus_dx = 0,
                    .bonus_dy = 2,
                    .vie_joueur = 0,
                    .vitesse_joueur = 2,   // Augmente la vitesse du joueur
                    .vitesse_balle = 0,
                    .score = 0
            },
            {
                    .nom = "Bonus Score",
                    .degats = 0,
                    .skin = "bonus_score.png",
                    .bonus_x = x,
                    .bonus_y = y,
                    .bonus_dx = 0,
                    .bonus_dy = 2,
                    .vie_joueur = 0,
                    .vitesse_joueur = 0,
                    .vitesse_balle = 0,
                    .score = 50            // Ajoute 50 points au score
            },
            {
                    .nom = "Malus Score",
                    .degats = 0,
                    .skin = "malus_score.png",
                    .bonus_x = x,
                    .bonus_y = y,
                    .bonus_dx = 0,
                    .bonus_dy = 3,
                    .vie_joueur = 0,
                    .vitesse_joueur = 0,
                    .vitesse_balle = 0,
                    .score = -50            // retire 50 points au score
            },
            {
                    .nom = "Bonus Super Balle",
                    .degats = 2,           // Double les dégâts de la balle
                    .skin = "bonus_super_balle.png",
                    .bonus_x = x,
                    .bonus_y = y,
                    .bonus_dx = 0,
                    .bonus_dy = 2,
                    .vie_joueur = 0,
                    .vitesse_joueur = 0,
                    .vitesse_balle = 0,
                    .score = 0
            },
            {
                    .nom = "Bonus Ralentissement",
                    .degats = 0,
                    .skin = "bonus_ralentissement.png",
                    .bonus_x = x,
                    .bonus_y = y,
                    .bonus_dx = 0,
                    .bonus_dy = 2,
                    .vie_joueur = 0,
                    .vitesse_joueur = 0,
                    .vitesse_balle = -2,   // Ralentit la balle
                    .score = 0
            }
    };

    // Nombre de bonus dans la liste
    int nombre_bonus = 6;

    // Sélection aléatoire d'un bonus
    int index = rand() % nombre_bonus;

    // Retourne le bonus sélectionné
    return bonus_list[index];
}

joueur init_joueur(joueur joueur){
    joueur.vitesse = 7;
    joueur.taille = 75;
    joueur.score = 0;
    return joueur;
}

balle init_balle(balle balle){
    balle.balle_x = WINDOW_X/2;
    balle.balle_y = WINDOW_Y - 40;
    balle.vitesse = 3;
    balle.balle_dx = balle.vitesse;
    balle.balle_dy = -balle.vitesse;
    balle.degats = 1;
    balle.skin = "balle1.png";
    return balle;
}

balle deplacement(balle balle, int** carte, int taille, joueur *joueur, bonus* tabonus, int *nbonus) {
    if (balle.balle_x >= WINDOW_X || balle.balle_x <= 0) {
        balle.balle_dx *= -1;
    } else if (balle.balle_y <= 0) {
        balle.balle_dy *= -1;
    } else if (balle.balle_y > WINDOW_Y - 40 && balle.balle_x > joueur->x && balle.balle_x < joueur->x + joueur->taille) {
        balle.balle_dy *= -1;
    } else if (balle.balle_y >= WINDOW_Y) {
        balle.balle_x = WINDOW_X / 2;
        degat_joueur(joueur);
        *joueur = init_joueur(*joueur);
        balle = init_balle(balle);
    } else if (carte[balle.balle_y / (WINDOW_Y / taille)][balle.balle_x / (WINDOW_X / taille)] > 0) {
        if (carte[(balle.balle_y - balle.vitesse) / (WINDOW_Y / taille)][balle.balle_x / (WINDOW_X / taille)] > 0) {
            balle.balle_dx *= -1;
        } else if (carte[balle.balle_y / (WINDOW_Y / taille)][(balle.balle_x - balle.vitesse) / (WINDOW_X / taille)] > 0) {
            balle.balle_dy *= -1;
        }
        if(carte[balle.balle_y / (WINDOW_Y / taille)][balle.balle_x / (WINDOW_X / taille)] -= balle.degats < 0){
            carte[balle.balle_y / (WINDOW_Y / taille)][balle.balle_x / (WINDOW_X / taille)] = 0;
        } else {
            carte[balle.balle_y / (WINDOW_Y / taille)][balle.balle_x / (WINDOW_X / taille)] -= balle.degats;
        }
        joueur->score += 1;

        // Ajout d'un bonus aléatoire
        if ((rand() % 10 + 1) == 1 && *nbonus < 40) { // Vérifiez que nbonus ne dépasse pas la taille de tabonus
            tabonus[*nbonus] = creer_bonus(balle.balle_x, balle.balle_y);
            (*nbonus)++; // Incrémentez nbonus
        }
    }

    // Déplacement des bonus et vérification des collisions avec le joueur
    for (int i = 0; i < (*nbonus); i++) {
        tabonus[i].bonus_x += tabonus[i].bonus_dx;
        tabonus[i].bonus_y += tabonus[i].bonus_dy;

        // Vérification de la collision entre le joueur et le bonus
        if (tabonus[i].bonus_y >= joueur->y && tabonus[i].bonus_y <= joueur->y + 20 &&
            tabonus[i].bonus_x >= joueur->x && tabonus[i].bonus_x <= joueur->x + joueur->taille) {
            // Appliquer les effets du bonus au joueur
            joueur->score += tabonus[i].score;
            joueur->vie += tabonus[i].vie_joueur;
            joueur->vitesse += tabonus[i].vitesse_joueur;
            if(balle.balle_dx > 0){
                balle.balle_dx += tabonus[i].vitesse_balle;
            } else {
                balle.balle_dx -= tabonus[i].vitesse_balle;
            }

            if(balle.balle_dy > 0){
                balle.balle_dy += tabonus[i].vitesse_balle;
            } else {
                balle.balle_dy -= tabonus[i].vitesse_balle;
            }

            // Supprimer le bonus du tableau
            for (int j = i; j < (*nbonus) - 1; j++) {
                tabonus[j] = tabonus[j + 1];
            }
            (*nbonus)--;
            i--; // Réajuster l'index après la suppression
        } else if (tabonus[i].bonus_y > WINDOW_Y){
            // Supprimer le bonus du tableau
            for (int j = i; j < (*nbonus) - 1; j++) {
                tabonus[j] = tabonus[j + 1];
            }
            (*nbonus)--;
            i--; // Réajuster l'index après la suppression
        }
    }

    balle.balle_x += balle.balle_dx;
    balle.balle_y += balle.balle_dy;
    return balle;
}


void game(SDL_Window *window, SDL_Renderer *renderer, joueur joueur, balle balle, int taille, int **carte, bonus* tabonus, int *nbonus) {
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
        afficher_fond(renderer, joueur);
        afficherCarte(carte, taille, renderer, joueur);
        afficherBonus(renderer, tabonus, *nbonus, taille);
        balle = deplacement(balle, carte, taille, &joueur, tabonus, nbonus); // Passez tabonus et nbonus
        afficherBalle(renderer, balle, taille);
        if (joueur.vie <= 0) {
            saisie = 0;
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
    SDL_Delay(200);
}

void menuPrincipal(SDL_Renderer *renderer, SDL_Window* window, int taille, balle balle1, bonus* tabonus, int nbonus, joueur joueur) {
    char* option[] = {"creer une carte", "charger une carte","Paremetre des bonus","Menu Adversaire"};
    int choixMenu = fen_QCM(option, 4, "");
    if (choixMenu == 0){
        afficher_fond(renderer, joueur);
        CreatCarte(taille, renderer, joueur);
    }
    else if (choixMenu == 1){
        int ** carte = chargeCarteDansTab(choixCarte(), 20);
        game(window, renderer, joueur, balle1, taille, carte, tabonus, &nbonus);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        libererCarte(carte, taille);
        free(carte);
        free(tabonus);
        menuPrincipal(renderer, window, taille, balle1, tabonus, nbonus, joueur);
    }else if(choixMenu == 2) {
        option[0] = "peronnaliser la balle";
        option[1] = "peronnaliser le joueur";
        switch (fen_QCM(option, 2, "paramètres")){
            case 0 :
                option[0] = "balle1.png";
                option[1] = "balle2.png";
                balle1.skin = fen_QCM2(option, 2, "choix balle", 50, 50);
                break;
            case 1 :
                option[0] = "joueur1.png";
                option[1] = "joueur2.png";
                joueur.skin = fen_QCM2(option, 2, "choix joueur", 100, 30);
                break;
        }
        menuPrincipal(renderer, window, taille, balle1, tabonus, nbonus, joueur);
    }else if(choixMenu == 3) {
        int menuAdversaire = fen_QCM(option, 2, "");
        if (menuAdversaire == 0) {
            int ** carte;
            carte = CreatCarteAdversaire(20, renderer, joueur);
            game(window, renderer, joueur, balle1, taille, carte, tabonus, &nbonus);
            carte = CreatCarteAdversaire(20, renderer, joueur);
            game(window, renderer, joueur, balle1, taille, carte, tabonus, &nbonus);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            libererCarte(carte, taille);
            free(carte);
            free(tabonus);
            menuPrincipal(renderer, window, taille, balle1, tabonus, nbonus, joueur);
        }else if (menuAdversaire == 1) {
            int ** carte = chargeCarteDansTab(choixCarte(), 20);
            game(window, renderer, joueur, balle1, taille, carte, tabonus, &nbonus);
        }
    }
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Window *window = SDL_CreateWindow("Fenêtre Bleue",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_X*1.20, WINDOW_Y,SDL_WINDOW_SHOWN);
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
    balle balle1 = init_balle(balle1);

    joueur joueur = init_joueur(joueur);
    joueur.x = WINDOW_X/2;
    joueur.y = WINDOW_Y - 30;
    joueur.vie = 1;
    joueur.skin = "joueur1.png";

    int nbonus = 0;
    bonus *tabonus = (bonus *)malloc(40 * sizeof(bonus));
    if (tabonus == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour tabonus.\n");
        exit(EXIT_FAILURE);
    }
    menuPrincipal(renderer, window, taille, balle1, tabonus, nbonus, joueur);
    return 0;
}