#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Struct for manipulation */
typedef struct
{
    unsigned int id;
    char nome[256];
    char sexo;
    float salario;
} funcionario;

/* Offsets */
#define ID 0
#define NOME ID + sizeof(unsigned int)
#define SEXO NOME + (sizeof(char) * 256)
#define SALARIO SEXO + sizeof(char) + 3
#define FUNCIONARIO sizeof(funcionario)

/* Global variables */
FILE *file;

/* Auxiliary types */
typedef int bool;
#define true 1
#define false 0

/* Foward definitions: operations */
void insert();
void delete ();
void averageSalaryForSex();
void dataToText();
void compress();

/* Foward definitions: helpers */
int offsetId(unsigned int id);
void question(char *quest, char *dest);

int main(int argc, char *argv[])
{

    if (argc < 3)
        return (-1);

    if ((file = fopen(argv[1], "r+b")) == NULL)
    {
        if ((file = fopen(argv[1], "w+b")) == NULL)
        {
            printf("Erro ao abrir/criar o arquivo.\n");
            return (-1);
        }
    }

    int option = atoi(argv[2]);

    switch (option)
    {
    case 0:
        insert();
        break;

    case 1:
        delete ();
        break;

    case 2:
        averageSalaryForSex();
        break;

    case 3:
        dataToText();
        break;

    default:
        printf("Do nothing...\n");
    }

    fclose(file);

    return 0;
}

int offsetId(unsigned int id)
{
    fseek(file, 0, SEEK_END);
    int end_of_file = ftell(file);

    if (!end_of_file)
        return -1;

    unsigned int aux_id = -1;
    int offset = 0;

    /* Beginning of file */
    fseek(file, 0, SEEK_SET);

    while (offset < end_of_file)
    {
        fread(&aux_id, sizeof(unsigned int), 1, file);

        printf("Off: %d ... Id: %u\n", offset, aux_id);
        if (id == aux_id)
            return offset;

        fseek(file, SALARIO, SEEK_CUR);
        offset = ftell(file);
    }

    return -1;
}

void question(char *quest, char *dest)
{
    printf("%s: ", quest);
    scanf("%s", dest);
}

void insert()
{
    char line[256];
    int id = -1, offset = 0;
    funcionario new_func;

    question("Informe o id", line);
    do
    {
        id = atoi(line);

        if (id != 0)
            offset = offsetId(id);

        if (offset == -1)
        {
            new_func.id = id;
            break;
        }

        question("\nId inválido, informe um Id diferente", line);
    } while (true);

    question("Informe o nome", line);
    strcpy(new_func.nome, line);

    question("Informe o sexo", line);
    do
    {
        if (line[0] == 'M' || line[0] == 'F')
        {
            new_func.sexo = line[0];
            break;
        }

        question("Sexo inválido, informe novamente", line);
    } while (true);

    question("Informe o salário", line);
    new_func.salario = atof(line);

    fseek(file, 0, SEEK_END);
    fwrite(&new_func, sizeof(funcionario), 1, file);

    printf("\nFuncionário inserido com sucesso.\n");
}

void delete ()
{
    char line[256];
    int id = -1, offset = -1;

    question("Informe o id", line);
    id = atoi(line);

    if (id > 0)
    {
        offset = offsetId(id);

        if (offset >= 0)
        {
            unsigned int removing = 0;

            fseek(file, offset, SEEK_SET);
            fwrite(&removing, sizeof(unsigned int), 1, file);

            printf("Funcionário removido com sucesso.\n");
            return;
        }
    }

    printf("Id inválido.\n");
}

void averageSalaryForSex()
{
    char sex = '\0', func_sex = '\0';
    int amount = 0, end_of_file = 0, offset = 0;
    float sum = 0, salary = 0;

    question("Informe o sexo para obter a média dos salário", &sex);

    while (!(sex == 'F' || sex == 'M'))
    {
        question("Sexo inválido, informe novamente", &sex);
    }

    /* Size of file */
    fseek(file, 0, SEEK_END);
    end_of_file = ftell(file);

    /* First value */
    fseek(file, 0, SEEK_SET);
    offset = ftell(file);

    while (offset < end_of_file)
    {
        fseek(file, offset + SEXO, SEEK_SET);
        fread(&func_sex, sizeof(char), 1, file);

        if (func_sex == sex)
        {
            fseek(file, offset + SALARIO, SEEK_SET);
            fread(&salary, sizeof(float), 1, file);

            amount++;
            sum += salary;
        }

        fseek(file, offset + FUNCIONARIO, SEEK_SET);
        offset = ftell(file);
    }

    printf("Média dos salários do sexo %c: %f\n", sex, (sum / amount));
}

void dataToText()
{
    FILE * text;
    char name[256];
    unsigned int id = 0;
    int offset = 0, end_of_file = 0;

    question("Informe o nome do arquivo para exportação dos dados", name);

    if ((text = fopen(name, "w+")) == NULL)
    {
        printf("Erro ao abrir/criar o arquivo.\n");
        return ;
    }

    /* Size of file */
    fseek(file, 0, SEEK_END);
    end_of_file = ftell(file);

    /* First value */
    fseek(file, 0, SEEK_SET);
    offset = ftell(file);
    
    while (offset < end_of_file)
    {
        fread(&id, sizeof(unsigned int), 1, file);

        if (id)
        {
            funcionario aux;
            char line[1024];

            fseek(file, offset, SEEK_SET);
            fread(&aux, sizeof(funcionario), 1, file);

            sprintf(line, "Id: %d - Nome: %s - Sexo: %c - Salario: %f\n", \
                    aux.id, aux.nome, aux.sexo, aux.salario);

            fputs(line, text);
        }

        fseek(file, offset + FUNCIONARIO, SEEK_SET);
        offset = ftell(file);
    }

    printf("Dados exportados com sucesso.\n");
}