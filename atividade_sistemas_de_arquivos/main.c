#include <stdio.h>

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
#define NOME sizeof(unsigned int)
#define SEXO NOME + (sizeof(char) * 256)
#define SALARIO SEXO + sizeof(char)
#define FUNCIONARIO SEXO + sizeof(float)

/* Global variables */
FILE *file;

/* Auxiliary types */
typedef int bool;
#define true 1
#define false 0

/* Foward definitions: operations */
void insert();
void delete();
void modify();
void compress();

/* Foward definitions: helpers */
int offsetId(unsigned int id);
void question(char * quest, char * dest);

int main(int argc, char *argv[])
{

    if (argc < 3)
        return (-1);

    if ((file = fopen(argv[1], "r+b")) == NULL)
    {
        if ((file = fopen(argv[1], "w+b")) == NULL)
        {
            printf("Error: Failed opening the file.\n");
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
        //remove(file);
        break;

    default:
        printf("Default...\n");
    }

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

        printf("\nOff: %d ... Id: %u", offset, aux_id);
        printf("\nId: %u ... offId: %u", id, aux_id);
        if (id == aux_id)
            return offset;

        fseek(file, FUNCIONARIO, SEEK_CUR);
        offset = ftell(file);

    }

    return -1;
}

void question(char * quest, char * dest)
{
    printf("\n%s: ", quest);
    scanf("%s", dest);
}

void insert()
{
    char line[256];
    int id = -1, offset;
    funcionario new_funcionario;
    
    question("Informe o id", line);

    do
    {
        id = atoi(line);
        offset = offsetId(id);

        if (offset == -1)
            break;

        question("Id inválido, informe um Id diferente", line);
    } while (true);

    new_funcionario.id = id;

    question("Informe o nome", line);
    strcpy(new_funcionario.nome, line);

    question("Informe o sexo", line);
    do
    {
        if (line[0] == 'M' || line[0] == 'F')
            break;

        question("Sexo inválido, informe novamente", line);
    } while (true);

    new_funcionario.sexo = line[0];
    

    question("Informe o salário", line);
    new_funcionario.salario = atof(line);

    fseek(file, 0, SEEK_END);
    fwrite(&new_funcionario, sizeof(funcionario), 1, file);

    printf("\nFuncionário inserido com sucesso.\n");
}