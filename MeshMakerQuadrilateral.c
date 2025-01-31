// Bibliotecas iniciais 

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h> // Para função unlink()
# include "hdf5.h"

const char *estruturas[] =            {"Meshes","Coordinates","Elements","Results"}; // array de ponteiros, em que cada ponteiro aponta para uma string diferente no array
const char *atributosMeshes[] =       {"Color", "Dimension", "ElemType","Name","Nnode"};
const char *atributosResults[] =      {"Analysis","Component 1","Component 2", "Component 3","Name", "NumComponents","ResultLocation","ResultType","Step"};             
const char *valorAtributosMeshes[] =  {"0 1 0 1","3","Quadrilateral","Group_0","4"};    
const char *valorAtributosResults[] = {"Phase Field","X-Displacement","Y-Displacement","Z-Displacement","Displacement","3","OnNodes","Vector","0"};
const char **atributos[] =            {atributosMeshes,atributosResults};  
const char **valorAtributos[] =       {valorAtributosMeshes,valorAtributosResults};

int iteracoesTotais =                 sizeof(estruturas)/sizeof(estruturas[0]); //  deve ser calculado antes de ser passado para a função, 
                                                                                //  já que "estruturas" decai para 'const char **' dentro da função
                                                                                //  e a logica de cálculo de tamanho não é mais válida 
int tamanhos[] =                      {sizeof(atributosMeshes)/sizeof(atributosMeshes[0]),sizeof(atributosResults)/sizeof(atributosResults[0])}; 

hid_t point1;
hid_t point2;
 /*
 - Testar Malha triangular
 */


void remove_arquivo(const char *nome_arquivo) 
{
    if (access(nome_arquivo, F_OK) == 0) 
    {
        if (unlink(nome_arquivo) != 0) {fprintf(stderr, "Erro ao remover o arquivo %s\n", nome_arquivo);}
    }
}

void distAtributos(const char *estruturas[], const char **atributos[],const char **valorAtributos[], int iteracoesTotais, int * tamanhos, double coords[4][9], int elements[6][4], int results[4][9]) 
{   
    // Identificação dimensões
    int numColsCoords = (sizeof(coords[0])/sizeof(coords[0][0])); //    sizeof(coords[0]) = 9 * 8 bytes (double); sizeof(coords[0][0]) = 8 bytes (double)
    int numColsElements = (sizeof(elements[0])/sizeof(elements[0][0])); 
    int numColsResults = (sizeof(results[0])/sizeof(results[0][0])); 

    // Criar datasets para paraview
    int elements_Paraview[4][numColsElements];
    for (int i = 1; i<5; i++)
    {
        for (int j = 0; j<numColsElements;j++)
        {
            elements_Paraview[i-1][j] = elements[i][j]-1;
        }
    }
    
    int results_Paraview[numColsResults][3];
    for (int i = 1; i<4;i++)
    {
        for (int j = 0; j<numColsResults; j++)
        {
            results_Paraview[j][i-1] = results[i][j];
        }
    }
    
    char H5file_name[] = "mesh.h5"; // Nome do arquivo
    herr_t status;                  // variável de retorno das operações de criação, abertura e fechamento

    remove_arquivo(H5file_name);
    hid_t H5file = H5Fcreate(H5file_name, H5F_ACC_TRUNC, H5P_DEFAULT,H5P_DEFAULT); // Criar arquivo HDF5
    if (H5file < 0) {fprintf(stderr, "Erro ao criar o arquivo HDF5.\n"); return;}

    char um[2] = "1"; // A padronização não aceita criar grupo com numeros como nome, então é preciso esse artifício
    char dois[2] = "2";
    char tres[2] = "3";
    
    // Criar atributos do root "/"  
    hid_t GrootId = H5Gopen(H5file, "/", H5P_DEFAULT); // Obter id do grupo "/"
    const char *AttrNameRoot = "GiD Post Results File"; // Nome do atributo
    const char *AttrValueRoot = "1.1";                  // Valor do atributo    
    hid_t StrRootType = H5Tcopy(H5T_C_S1);                 // Tipo do atributo (string de tamanho 1)
    H5Tset_size(StrRootType, 3);                           // Tamanho da string (3)
    H5Tset_strpad(StrRootType, H5T_STR_NULLTERM);          // String será preenchida com caracteres nulos até o tamanho especificado.
    hid_t SpaceRootId = H5Screate(H5S_SCALAR);             // Espaço de dados associado aos atributos do root
    hid_t AttrRootId = H5Acreate(GrootId, AttrNameRoot, StrRootType, SpaceRootId, H5P_DEFAULT, H5P_DEFAULT); // Criar o atributo
    H5Awrite(AttrRootId, StrRootType, AttrValueRoot);         // Escrever o valor do atributo
    H5Aclose(AttrRootId);                                  // Fecha atributo                              
    H5Sclose(SpaceRootId);                                 // Fecha dataspace
    H5Tclose(StrRootType);                                 // Fecha datatype

    // Criar grupos em uma estruturas padrão
    
    hid_t H5Gmeshes = H5Gcreate(H5file, "/Meshes", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (H5Gmeshes < 0) {fprintf(stderr, "Erro ao criar o grupo /Meshes.\n");H5Gclose(H5Gmeshes);H5Fclose(H5file);return;}
    
    hid_t H5Gresults = H5Gcreate(H5file, "/Results", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (H5Gresults < 0) {fprintf(stderr, "Erro ao criar o grupo /Results.\n");H5Gclose(H5Gresults); H5Fclose(H5file);return;}
    
    for (int j = 0; j < iteracoesTotais; j++)
    {
        if(strcmp(estruturas[j],"Meshes")==0) 
        {   
            hid_t GrupoM = H5Gcreate(H5Gmeshes, um, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);                 
            point1 = GrupoM;
            
            int k = 0; //No caso de meshes
            for (int i = 0; i < tamanhos[k] ; i++) 
            {
                char AttrName[50];
                char AttrValue[50];
                strcpy(AttrName,atributos[k][i]);                            // atributos[i] e uma string 
                strcpy(AttrValue,valorAtributos[k][i]);                      // valorAtributos[i] e uma string
            
                hid_t StrType = H5Tcopy(H5T_C_S1);
                H5Tset_size(StrType, strlen(AttrValue)+1);
                H5Tset_strpad(StrType, H5T_STR_NULLTERM);
                
                hid_t SpaceId = H5Screate(H5S_SCALAR);                          // Pode ser que seja vetor, neh?
                hid_t AttrId = H5Acreate(GrupoM, AttrName, StrType, SpaceId, H5P_DEFAULT, H5P_DEFAULT);
                
                H5Awrite(AttrId, StrType, AttrValue);    
                H5Aclose(AttrId);                                                  
                H5Sclose(SpaceId);                                 
                H5Tclose(StrType); 
            }
            // colocar if for para verificar se ha coordinates e elements ainda pra fazer
        }

        else if(strcmp(estruturas[j],"Coordinates")==0)
        {
            hid_t GrupoC = H5Gcreate(point1,"Coordinates", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            
            // Datasets para as coordenadas 
            hid_t coords_dataspace = H5Screate_simple(1, (hsize_t[]){numColsCoords}, NULL); // Cria dataspace para as coordenadas
            hid_t coords_datatype = H5Tcopy(H5T_NATIVE_DOUBLE);                 // Cria datatype para o dataset
            hid_t coords_dataset1 = H5Dcreate(GrupoC, "1", H5T_STD_I32LE, coords_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);    // Cria datasets
            hid_t coords_dataset2 = H5Dcreate(GrupoC, "2", coords_datatype, coords_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t coords_dataset3 = H5Dcreate(GrupoC, "3", coords_datatype, coords_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t coords_dataset4 = H5Dcreate(GrupoC, "4", coords_datatype, coords_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            status = H5Dwrite(coords_dataset1, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, coords[0]);                              // Aloca informações no dataset
            status = H5Dwrite(coords_dataset2, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, coords[1]);
            status = H5Dwrite(coords_dataset3, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, coords[2]);
            status = H5Dwrite(coords_dataset4, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, coords[3]);
            H5Tclose(coords_datatype);      // Fecha datatype
            H5Sclose(coords_dataspace);     // Fecha dataspace
            H5Dclose(coords_dataset1);      // Fecha datasets
            H5Dclose(coords_dataset2);
            H5Dclose(coords_dataset3);
            H5Dclose(coords_dataset4);
            
            H5Gclose(GrupoC);
        }

        else if(strcmp(estruturas[j],"Elements")==0)
        { 
            hid_t GrupoE = H5Gcreate(point1,"Elements", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            
            // Datasets para as incidências dos elementos 
            hid_t elements_dataspace = H5Screate_simple(1, (hsize_t[]){numColsElements}, NULL);   
            hsize_t dims_elements[2] = {4, 4};
            hid_t elementsParaview_dataspace = H5Screate_simple(2, dims_elements, NULL);   
            hid_t elements_datatype = H5Tcopy(H5T_NATIVE_INT);
            hid_t elements_dataset1 = H5Dcreate(GrupoE, "1", elements_datatype, elements_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t elements_dataset2 = H5Dcreate(GrupoE, "2", elements_datatype, elements_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t elements_dataset3 = H5Dcreate(GrupoE, "3", elements_datatype, elements_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t elements_dataset4 = H5Dcreate(GrupoE, "4", elements_datatype, elements_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t elements_dataset5 = H5Dcreate(GrupoE, "5", elements_datatype, elements_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t elements_dataset6 = H5Dcreate(GrupoE, "6", elements_datatype, elements_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t elements_dataset_IncidParaview = H5Dcreate(GrupoE, "IncidParaview", elements_datatype, elementsParaview_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            status = H5Dwrite(elements_dataset1, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, elements[0]);
            status = H5Dwrite(elements_dataset2, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, elements[1]);
            status = H5Dwrite(elements_dataset3, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, elements[2]);
            status = H5Dwrite(elements_dataset4, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, elements[3]);
            status = H5Dwrite(elements_dataset5, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, elements[4]);
            status = H5Dwrite(elements_dataset6, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, elements[5]);
            status = H5Dwrite(elements_dataset_IncidParaview, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,elements_Paraview);
            
            H5Tclose(elements_datatype);
            H5Sclose(elements_dataspace);
            H5Sclose(elementsParaview_dataspace);
            H5Dclose(elements_dataset_IncidParaview);
            H5Dclose(elements_dataset6);
            H5Dclose(elements_dataset5);
            H5Dclose(elements_dataset4);
            H5Dclose(elements_dataset3);
            H5Dclose(elements_dataset2);
            H5Dclose(elements_dataset1);
            
            H5Gclose(GrupoE);
        }

        else if(strcmp(estruturas[j],"Results")==0)
        {
            hid_t GrupoR = H5Gcreate(H5Gresults, um, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            point2 = GrupoR;

            hid_t results_dataspace = H5Screate_simple(1, (hsize_t[]){numColsResults}, NULL);
            hsize_t dims_results[2] = {numColsResults,3};
            hid_t results_dataspace_Paraview = H5Screate_simple(2, dims_results, NULL);
            hid_t results_datatype = H5Tcopy(H5T_NATIVE_DOUBLE);
            hid_t results_dataset1 = H5Dcreate(GrupoR, "1", H5T_NATIVE_INT, results_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t results_dataset2 = H5Dcreate(GrupoR, "2", results_datatype, results_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t results_dataset3 = H5Dcreate(GrupoR, "3", results_datatype, results_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t results_dataset4 = H5Dcreate(GrupoR, "4", results_datatype, results_dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            hid_t results_dataset_Paraview = H5Dcreate(GrupoR, "ResultParaview", results_datatype, results_dataspace_Paraview, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            
            status = H5Dwrite(results_dataset1, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, results[0]);
            status = H5Dwrite(results_dataset2, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, results[1]);
            status = H5Dwrite(results_dataset3, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, results[2]);
            status = H5Dwrite(results_dataset4, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, results[3]);
            status = H5Dwrite(results_dataset_Paraview, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, results_Paraview);
            
            
            H5Tclose(results_datatype);
            H5Sclose(results_dataspace_Paraview);
            H5Sclose(results_dataspace);
            H5Dclose(results_dataset1);
            H5Dclose(results_dataset2);
            H5Dclose(results_dataset3);
            H5Dclose(results_dataset4);
            H5Dclose(results_dataset_Paraview);
            
            int k = 1; //No caso de results
            for (int i = 0; i < tamanhos[k] ; i++) 
            {
                char AttrName[50];
                char AttrValue[50];
                strcpy(AttrName,atributos[k][i]);                            // atributos[i] e uma string 
                strcpy(AttrValue,valorAtributos[k][i]);                      // valorAtributos[i] e uma string
            
                hid_t StrType = H5Tcopy(H5T_C_S1);
                H5Tset_size(StrType, strlen(AttrValue)+1);
                H5Tset_strpad(StrType, H5T_STR_NULLTERM);
                
                hid_t SpaceId = H5Screate(H5S_SCALAR);                          // Pode ser que seja vetor, neh?
                hid_t AttrId = H5Acreate(GrupoR, AttrName, StrType, SpaceId, H5P_DEFAULT, H5P_DEFAULT);
                
                H5Awrite(AttrId, StrType, AttrValue);    
                H5Aclose(AttrId);                                                  
                H5Sclose(SpaceId);                                 
                H5Tclose(StrType); 
            }
            ;
        }
    }

    // Finaliza acesso ao arquivo
    //status = H5Gclose(H5Gcoords);
    //status = H5Gclose(H5Gelements);
    //status = H5Gclose(H5Gmesh1);
    
    status = H5Gclose(point2);
    status = H5Gclose(point1);
    status = H5Gclose(H5Gmeshes);
    status = H5Gclose(H5Gresults);
    status = H5Fclose(H5file); 
}

int main() {
    
    // Criar malha 

    // Coordenadas
    //int n_no_ele = 4;
    //int n_elem = 4;
    //int n_nos = 9;
    double coords[4][9] = { /*indice de cada nó*/ {1,2,3,4,5,6,7,8,9},
                            /*coordenadas em x */ {0.0,0.5,1.0,0.0,0.5,1.0,0.0,0.5,1.0},
                            /*coordenadas em y */ {0.0,0.0,0.0,0.5,0.5,0.5,1.0,1.0,1.0},
                            /*coordenadas em z */ {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}};

    // Incidencia de elementos
    int elements [6][4] = { /*indice de cada elemento*/         {1,2,3,4},
                            /*indice 1 do nó de cada elemento*/ {1,2,5,4},
                            /*indice 2 do nó de cada elemento*/ {2,3,6,5},
                            /*indice 3 do nó de cada elemento*/ {5,6,9,8},
                            /*indice 4 do nó de cada elemento*/ {4,5,8,7},
                            /*vetor unitario*/                  {1,1,1,1}};

    // Resultado
    int results[4][9] = { /*indice de cada nó*/         {1,2,3,4,5,6,7,8,9},
                          /*resultado de cada nó em x*/ {0,1,2,1,1,2,2,2,2},
                          /*resultado de cada nó em y*/ {0,1,2,1,1,2,2,2,2},
                          /*resultado de cada nó em y*/ {0,1,2,1,1,2,2,2,2}};

    distAtributos(estruturas,atributos,valorAtributos,iteracoesTotais,tamanhos, coords, elements, results);

    return 0;

}

