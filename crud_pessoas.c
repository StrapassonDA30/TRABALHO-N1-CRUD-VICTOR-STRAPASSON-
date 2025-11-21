#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ARQUIVO "pessoas.bin"
#define TEMP "temp.bin"

// =====================
//       STRUCT
// =====================
typedef struct {
    char nome[100];
    char cpf[12];  // 11 dígitos + '\0'
    int idade;
    char email[100];
} Pessoa;

// =====================
//   FUNÇÕES AUXILIARES
// =====================

// Ler string de forma segura
void read_line(char *buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

// Verificar se todos caracteres são números
int only_digits(const char *s) {
    for (int i = 0; s[i]; i++)
        if (!isdigit(s[i])) return 0;
    return 1;
}

// Validar CPF (somente formato e dígitos)
int validar_cpf(const char *cpf) {
    return strlen(cpf) == 11 && only_digits(cpf);
}

// Verificar se CPF existe no arquivo
int cpf_existe(const char *cpf) {
    FILE *f = fopen(ARQUIVO, "rb");
    if (!f) return 0;

    Pessoa p;
    while (fread(&p, sizeof(Pessoa), 1, f) == 1) {
        if (strcmp(p.cpf, cpf) == 0) {
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

// =====================
//       CREATE
// =====================
void cadastrar() {
    Pessoa p;
    char idade_str[10];

    printf("\n--- CADASTRAR PESSOA ---\n");

    printf("Nome: ");
    read_line(p.nome, 100);

    printf("CPF (11 dígitos): ");
    read_line(p.cpf, 12);

    if (!validar_cpf(p.cpf)) {
        printf("[ERRO] CPF inválido!\n");
        return;
    }

    if (cpf_existe(p.cpf)) {
        printf("[ERRO] CPF já cadastrado!\n");
        return;
    }

    printf("Idade: ");
    read_line(idade_str, 10);
    p.idade = atoi(idade_str);

    printf("Email: ");
    read_line(p.email, 100);

    FILE *f = fopen(ARQUIVO, "ab");
    fwrite(&p, sizeof(Pessoa), 1, f);
    fclose(f);

    printf("[OK] Pessoa cadastrada com sucesso!\n");
}

// =====================
//        READ
// =====================
void listar() {
    FILE *f = fopen(ARQUIVO, "rb");
    if (!f) {
        printf("\nNenhuma pessoa cadastrada.\n");
        return;
    }

    Pessoa p;
    printf("\n--- LISTA DE PESSOAS ---\n");
    while (fread(&p, sizeof(Pessoa), 1, f) == 1) {
        printf("\nNome : %s\nCPF  : %s\nIdade: %d\nEmail: %s\n",
               p.nome, p.cpf, p.idade, p.email);
    }

    fclose(f);
}

void buscar_cpf() {
    char cpf[12];

    printf("\nDigite o CPF para buscar: ");
    read_line(cpf, 12);

    if (!validar_cpf(cpf)) {
        printf("[ERRO] CPF inválido!\n");
        return;
    }

    FILE *f = fopen(ARQUIVO, "rb");
    if (!f) {
        printf("\nNenhum registro encontrado.\n");
        return;
    }

    Pessoa p;
    while (fread(&p, sizeof(Pessoa), 1, f) == 1) {
        if (strcmp(cpf, p.cpf) == 0) {
            printf("\n--- RESULTADO ---\n");
            printf("Nome : %s\nCPF  : %s\nIdade: %d\nEmail: %s\n",
                   p.nome, p.cpf, p.idade, p.email);
            fclose(f);
            return;
        }
    }

    printf("[ERRO] CPF não encontrado.\n");
    fclose(f);
}

// =====================
//        UPDATE
// =====================
void atualizar() {
    char cpf[12];
    char buffer[200];

    printf("\nDigite o CPF para atualizar: ");
    read_line(cpf, 12);

    if (!validar_cpf(cpf)) {
        printf("[ERRO] CPF inválido!\n");
        return;
    }

    FILE *f = fopen(ARQUIVO, "rb+");
    if (!f) {
        printf("[ERRO] Nenhum arquivo encontrado.\n");
        return;
    }

    Pessoa p;
    int encontrado = 0;
    long pos;

    while (fread(&p, sizeof(Pessoa), 1, f) == 1) {
        if (strcmp(p.cpf, cpf) == 0) {
            encontrado = 1;
            pos = ftell(f) - sizeof(Pessoa);
            break;
        }
    }

    if (!encontrado) {
        printf("[ERRO] CPF não encontrado.\n");
        fclose(f);
        return;
    }

    printf("\n--- DADOS ATUAIS ---\n");
    printf("Nome : %s\nCPF  : %s\nIdade: %d\nEmail: %s\n",
           p.nome, p.cpf, p.idade, p.email);

    printf("\nNovo nome (ENTER = manter): ");
    read_line(buffer, 200);
    if (strlen(buffer) > 0) strcpy(p.nome, buffer);

    printf("Nova idade (ENTER = manter): ");
    read_line(buffer, 200);
    if (strlen(buffer) > 0) p.idade = atoi(buffer);

    printf("Novo email (ENTER = manter): ");
    read_line(buffer, 200);
    if (strlen(buffer) > 0) strcpy(p.email, buffer);

    fseek(f, pos, SEEK_SET);
    fwrite(&p, sizeof(Pessoa), 1, f);

    fclose(f);

    printf("[OK] Registro atualizado!\n");
}

// =====================
//        DELETE
// =====================
void remover() {
    char cpf[12];

    printf("\nDigite o CPF para remover: ");
    read_line(cpf, 12);

    if (!validar_cpf(cpf)) {
        printf("[ERRO] CPF inválido!\n");
        return;
    }

    FILE *f = fopen(ARQUIVO, "rb");
    FILE *temp = fopen(TEMP, "wb");

    if (!f || !temp) {
        printf("[ERRO] Não foi possível abrir arquivos.\n");
        return;
    }

    Pessoa p;
    int removido = 0;

    while (fread(&p, sizeof(Pessoa), 1, f) == 1) {
        if (strcmp(p.cpf, cpf) == 0) {
            removido = 1;
            continue;
        }
        fwrite(&p, sizeof(Pessoa), 1, temp);
    }

    fclose(f);
    fclose(temp);

    remove(ARQUIVO);
    rename(TEMP, ARQUIVO);

    if (removido)
        printf("[OK] Registro removido.\n");
    else
        printf("[ERRO] CPF não encontrado.\n");
}

// =====================
//        MENU
// =====================
int main() {
    int op;
    char op_str[10];

    do {
        printf("\n===== SISTEMA CRUD =====\n");
        printf("1. Cadastrar pessoa\n");
        printf("2. Listar pessoas\n");
        printf("3. Buscar por CPF\n");
        printf("4. Atualizar cadastro\n");
        printf("5. Remover cadastro\n");
        printf("0. Sair\n");
        printf("Escolha: ");

        read_line(op_str, 10);
        op = atoi(op_str);

        switch (op) {
            case 1: cadastrar(); break;
            case 2: listar(); break;
            case 3: buscar_cpf(); break;
            case 4: atualizar(); break;
            case 5: remover(); break;
            case 0: printf("Saindo...\n"); break;
            default: printf("[ERRO] Opção inválida!\n");
        }

    } while (op != 0);

    return 0;
}
