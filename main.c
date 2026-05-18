#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLU     "\033[34m"
#define RESET   "\033[0m"

struct FileNode {
    char name[100];
    char type;

    union {
        char *content;           // Se è un file (testo dinamico)
        struct FileNode *child;  // Se è una directory (punta al primo figlio)
    } data;

    struct FileNode *parent;
    struct FileNode *siblings;

};

struct LevelNode {
    int level;
    int hasSiblings;
    struct LevelNode *next;
};


void touch(struct FileNode *head, char name[100]){
    if(head->type!='D'){
        printf("File can be created only in a directory");
        return;
    }
    struct FileNode *newFile = malloc(sizeof(struct FileNode));
    strcpy(newFile->name, name);
    
    struct FileNode *curr = head;

    newFile->parent = curr;
    newFile->siblings = NULL;
    newFile->type = 'F';

    if(curr->data.child==NULL){
        curr->data.child = newFile;
    }else{
        curr = curr->data.child;

        while (curr->siblings!=NULL){
            curr= curr->siblings;
        }
        
        curr->siblings = newFile;
    }
}

void ls(struct FileNode *head){
    if(head->data.child == NULL) return;
    struct FileNode *curr = head;
    curr = curr->data.child;
    if(curr->type == 'D'){
        printf(BLU);
    }else{
        printf(RED);
    }
    printf("%s ", curr->name);
    while(curr->siblings!=NULL){
        curr = curr->siblings;

        if(curr->type == 'D'){
            printf(BLU);
        }else{
            printf(RED);
        }
        printf("%s ", curr->name);
    }

    printf("\n" RESET);
}

struct FileNode *cd(struct FileNode *head, char name[100]){
    struct FileNode *cwd = head;
    if(strcmp(name, "..")==0){
        if(cwd->parent!=NULL) return cwd->parent;
        else return head;
    }else if(cwd->data.child==NULL){
        printf("No such directory\n");
        return head;
    }else{
        cwd=cwd->data.child;
        while (cwd != NULL && strcmp(cwd->name, name) != 0){
            cwd = cwd->siblings;
        }

        if (cwd != NULL)
        {
            if(cwd->type == 'D'){
                return cwd;
            }else{
                printf("Cannot cd into a file\n");
                return head;
            }
        }else{
            printf("No such directory\n");
            return head;
        } 
    }
}

void printChilds(struct FileNode **head, int *i, int siblingsLev[100], FILE *f){
    struct FileNode *curr = *head;
    
    //stampa su file
    if(f!=NULL){
        for (int j = 1; j < *i; j++) {
            if (siblingsLev[j] == 1) fprintf(f, "│   ");
            else                     fprintf(f, "    ");
        }
            
        // Stampa il simbolo finale per il nodo corrente
        if (curr->siblings == NULL) fprintf(f, "└── ");
        else                        fprintf(f, "├── ");
        if(curr->type=='D') fprintf(f, "%s/\n", curr->name);
        else fprintf(f, "%s\n", curr->name);
        

        // 2. Discesa in profondità sui figli
        while (curr->data.child != NULL) {
            // Registriamo se il padre ha fratelli PRIMA di scendere di livello
            if (curr->siblings != NULL) siblingsLev[*i] = 1;
            else                        siblingsLev[*i] = 0;

            (*i)++;
            curr = curr->data.child;

            // Il ciclo controlla TUTTI i livelli precedenti (da 1 a *i - 1)
            for (int j = 1; j < *i; j++) {
                if (siblingsLev[j] == 1) fprintf(f, "│   ");
                else                     fprintf(f, "    ");
            }
            
            // Stampa il simbolo finale per il figlio corrente
            if (curr->siblings == NULL) fprintf(f, "└── ");
            else                        fprintf(f, "├── ");            
            
            if(curr->type=='D') fprintf(f, "%s/\n", curr->name);
            else fprintf(f, "%s\n", curr->name);
        }
        
        *head = curr;
    
    //stampa si terminale
    }else{
    // Se il livello ha fratelli stampa │, altrimenti lascia lo spazio vuoto
        for (int j = 1; j < *i; j++) {
            if (siblingsLev[j] == 1) printf("│   ");
            else                     printf("    ");
        }
            
        // Stampa il simbolo finale per il nodo corrente
        if (curr->siblings == NULL) printf("└── ");
        else                        printf("├── ");
        if(curr->type=='D') printf("%s/\n", curr->name);
        else printf("%s\n", curr->name);
        

        // 2. Discesa in profondità sui figli
        while (curr->data.child != NULL) {
            // Registriamo se il padre ha fratelli PRIMA di scendere di livello
            if (curr->siblings != NULL) siblingsLev[*i] = 1;
            else                        siblingsLev[*i] = 0;

            (*i)++;
            curr = curr->data.child;

            // Il ciclo controlla TUTTI i livelli precedenti (da 1 a *i - 1)
            for (int j = 1; j < *i; j++) {
                if (siblingsLev[j] == 1) printf("│   ");
                else                     printf("    ");
            }
            
            // Stampa il simbolo finale per il figlio corrente
            if (curr->siblings == NULL) printf("└── ");
            else                        printf("├── ");
            
            if(curr->type=='D') printf("%s/\n", curr->name);
            else printf("%s\n", curr->name);
        }
    
        *head = curr;
    }
}

void saveOnFile(struct FileNode * head){
    FILE *f = fopen("system.txt", "w");
    if (head == NULL) return;
    
    struct FileNode *curr = head;
    int i = 0;
    int siblingsLev[100] = {0}; 
    
    // Stampa il nome della radice a livello 0 (senza rami davanti)
    fprintf(f, "%s\n", curr->name);
    
    if (curr->data.child != NULL) {
        i++;
        curr = curr->data.child;
        printChilds(&curr, &i, siblingsLev, f);

        while (head != curr) {
            while (curr->siblings != NULL) {
                curr = curr->siblings;
                if (curr->siblings != NULL){ 
                    // Prima di chiamare printChilds, aggiorna lo stato del livello attuale per verificare se ci sono fratelli:
                    siblingsLev[i] = 1;
                }else{
                    // se questo nuovo fratello è l'ultimo, la linea non ci deve piu essere
                    siblingsLev[i] = 0;
                }
                
                printChilds(&curr, &i, siblingsLev, f);
            }
            // Quando risali, pulisci la memoria del livello che stai lasciando
            siblingsLev[i] = 0;
            i--;
            curr = curr->parent;
        }
    }
    fclose(f);    
}

void mkdir(struct FileNode *head, char name[100]){
    struct FileNode *newDir = malloc(sizeof(struct FileNode));
    strcpy(newDir->name, name);
    struct FileNode * curr = head;
    newDir->parent = curr;
    newDir->data.child = NULL;
    newDir->siblings = NULL;
    newDir->type = 'D';

    if(curr->data.child==NULL){
        curr->data.child = newDir;
    }else{
        curr = curr->data.child;

        while (curr->siblings!=NULL){
            curr= curr->siblings;
        }
        
        curr->siblings = newDir;
    }
}

void tree(struct FileNode *head){
    if (head == NULL) return;
    
    struct FileNode *curr = head;
    int i = 0;
    int siblingsLev[100] = {0}; 
    
    // Stampa il nome della radice a livello 0 (senza rami davanti)
    printf("%s\n", curr->name);
    
    if (curr->data.child != NULL) {
        i++;
        curr = curr->data.child;
        printChilds(&curr, &i, siblingsLev, NULL);

        while (head != curr) {
            while (curr->siblings != NULL) {
                curr = curr->siblings;
                if (curr->siblings != NULL){ 
                    // Prima di chiamare printChilds, aggiorna lo stato del livello attuale per verificare se ci sono fratelli:
                    siblingsLev[i] = 1;
                }else{
                    // se questo nuovo fratello è l'ultimo, la linea non ci deve piu essere
                    siblingsLev[i] = 0;
                }
                
                printChilds(&curr, &i, siblingsLev, NULL);
            }
            // Quando risali, pulisci la memoria del livello che stai lasciando
            siblingsLev[i] = 0;
            i--;
            curr = curr->parent;
        }
    }
}

// Questa funzione conta i CARATTERI reali consentiti prima di fermarsi
int contaCaratteriPrefisso(const char *line) {
    const char *caratteri_consentiti = "│├└─\xC2 \xA0"; // Caratteri speciali grafici + spazio normale + spazio non-breaking
    int charCount = 0;
    int i = 0;

    while (line[i] != '\0') {
        // Controlliamo se il byte corrente è l'inizio di un vero carattere UTF-8
        if (((unsigned char)line[i] & 0xC0) != 0x80) {
            
            // Verifichiamo se questo carattere (o il suo byte iniziale) è tra quelli consentiti.
            // strchr cerca se il byte line[i] è presente nella stringa dei consentiti.
            if (strchr(caratteri_consentiti, line[i]) != NULL) {
                charCount++; // Il carattere è consentito, lo contiamo come 1 solo
            } else {
                // Abbiamo trovato un carattere NON consentito (es. una lettera o un numero)
                break; 
            }
        }
        i++; // Avanza al byte successivo
    }
    printf("Caratteri di prefisso contati: %d\n", charCount);
    return charCount;
}

void initializeFileSystem(struct FileNode *root){
    FILE *f = fopen("system.txt", "r");
    if (!f) {
        printf("Errore nell'apertura del file.\n");
        return;
    }

    char line[500];
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return;
    }
    
    // Salva il nome del root (rimuovendo il primo carattere es. '/' o '\')
    line[strcspn(line, "\n")] = 0;
    strcpy(root->name, &line[1]);
    
    int preLevel = 0;
    struct FileNode *curr = root;
    
    while(fgets(line, sizeof(line), f)){
        //- rimuovo i carattere del 'a capo'
        line[strcspn(line, "\n")] = 0;
        
        //trovo quanti caratteri ci sono prima del nome e dividendoli per 4 trovo il livello
        int level = contaCaratteriPrefisso(line)/4; 
        
        //trovo quanti byte c ìi sono prima del nome
        int prefixBytes = strspn(line, "│├└─\xC2 \xA0"); 
        // utilizzo i byte per ttrovare l'indice del primo carattere del nome
        char *nodeName = &line[prefixBytes];
        
        //* RISALITA: torno indietro nell'albero fino a quando non arrivo al livello del
        //* padre dell'elemento 
        for(preLevel; preLevel>=level; preLevel--) {
            curr = curr->parent;
        }
        
        //verifico se l'ultimo carattere del nome mi identifica una cartella
        if(nodeName[strlen(nodeName)-1] == '/'){
            //-elimino l'identificazione della cartella
            nodeName[strlen(nodeName)-1] = '\0';
            mkdir(curr, nodeName);
            //* mi sposto all'interno della nuova cartella, quindi, vado al livello della nuova dir
            curr = cd(curr, nodeName);
            preLevel=level; 
        }else{
            //-creo il file nella cartella corrente
            touch(curr, nodeName);
        }
    }
}

int main(){
    struct FileNode *U = malloc(sizeof(struct FileNode));
    U->type='D';
    U->data.child = NULL;
    U->parent = NULL;
    U->siblings = NULL;

    initializeFileSystem(U);

    int running = 1;
    while(running){
        int parents = 0;
        struct FileNode *curr = U;
        while (curr->parent != NULL){
            parents+=1;
            curr = curr->parent;      
        }    

        int rootPrinted = 0;
        while(parents>=0){
            curr = U;
            for(int i=parents-1; i>=0; i--){
                curr = curr->parent;
            }  
            if (rootPrinted == 0){
                printf(GREEN);
                rootPrinted = 1;
            } else {
                printf(BLU "/");
            }
            printf("%s", curr->name);  
            parents--;        
        }
        printf(RESET "-$ ");

        //printa i +l pirmo figlio di U
        
        char input[100]; 
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        char *token = strtok(input, " ");

        if(strcmp(token, "mkdir")==0){
            mkdir(U, strtok(NULL, " "));
            printf("Directory creata\n");
            //stampa nome e genitore
            struct FileNode *curr = U;
            while (curr->parent != NULL) curr = curr->parent;
            saveOnFile(curr);
        }else if(strcmp(token, "touch")==0){
            touch(U, strtok(NULL, " "));
            printf("file created");
            //!SaveOnFile da errore nonostante il file lo crei e lo riesce a salvare dopo che faccio mkdir (RISOLVERE)
            //saveOnFile(U);
        }else if(strcmp(token, "ls")==0){
            ls(U);
        }else if(strcmp(token, "cd")==0){
            U = cd(U, strtok(NULL, " "));
        }else if(strcmp(token, "tree")==0){
            tree(U);
        }
    }
}