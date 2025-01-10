 # include <stdio.h>
 #include "criarArquivoXDMF.h" // Inclui o cabeçalho da função

 void criarArquivoXDMF() {

const char *filename = "mesh.xdmf";
FILE *arquivo = fopen(filename,"w");

int NumNodes = 9;
int NodesPerElement = 4;
int NumElements = 4;
const char CompletePath[] = "C:/Users/arthu/Desktop/Codigos/mesh.h5";

fprintf(arquivo,"<?xml version=\"1.0\"?>\n");
fprintf(arquivo,"<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\" []>\n");
fprintf(arquivo,"<Xdmf Version=\"3.0\" xmlns:xi=\"http://www.w3.org/2001/XInclude\">\n");
fprintf(arquivo,"  <Domain>\n");
fprintf(arquivo,"    <Grid Name=\"TimeSeries\" GridType=\"Collection\" CollectionType=\"Temporal\">\n");
fprintf(arquivo,"      <Grid Name=\"mesh\" GridType=\"Uniform\">\n");
fprintf(arquivo,"        <Geometry GeometryType=\"X_Y_Z\">\n");
fprintf(arquivo,"          <DataItem Dimensions=\"%d 1\" Format=\"HDF\">%s:/Meshes/1/Coordinates/2</DataItem>\n",NumNodes,CompletePath);
fprintf(arquivo,"          <DataItem Dimensions=\"%d 1\" Format=\"HDF\">%s:/Meshes/1/Coordinates/3</DataItem>\n",NumNodes,CompletePath);
fprintf(arquivo,"          <DataItem Dimensions=\"%d 1\" Format=\"HDF\">%s:/Meshes/1/Coordinates/4</DataItem>\n",NumNodes,CompletePath);
fprintf(arquivo,"        </Geometry>\n");
fprintf(arquivo,"        <Topology NumberOfElements=\"%d\" TopologyType=\"Quadrilateral\" NodesPerElement=\"%d\">\n", NumElements,NodesPerElement);
fprintf(arquivo,"          <DataItem Dimensions=\"%d %d\" NumberType=\"UInt\" Format=\"HDF\">%s:/Meshes/1/Elements/IncidParaview</DataItem>\n",NumElements,NodesPerElement,CompletePath);
fprintf(arquivo,"        </Topology>\n");
fprintf(arquivo,"        <Time Value=\"1.000000e+00\" />\n");
fprintf(arquivo,"        <Attribute Name=\"Displacement\" AttributeType=\"Vector\" Center=\"Node\">\n");
fprintf(arquivo,"          <DataItem Dimensions=\"3 %d\" Format=\"HDF\">%s:/Results/1/ResultParaview</DataItem>\n",NumNodes,CompletePath);
fprintf(arquivo,"        </Attribute>\n");
fprintf(arquivo,"      </Grid>\n");
fprintf(arquivo,"    </Grid>\n");
fprintf(arquivo,"  </Domain>\n");
fprintf(arquivo,"</Xdmf>");
fclose(arquivo); 
}