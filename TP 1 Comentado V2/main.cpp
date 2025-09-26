#include <iostream>
#include <fstream>
using namespace std;

//Estructura para guardar el tipo de producto a reparar
struct producto{
    string SKU;
    string descripcion;
    float costofijo;
};

//Estructura para guardar las reparaciones listas
struct reparaciones{
    string cliente;
    int tiproduct;
    string SKU;
    float costoDirecto;
    float presupuestado;
};

//Dimensiones de los datos de tipo string de las estructuras, estos seran usados para deserializar los datos
const int dimSKU {10};
const int dimdescr {20};
char buffer[50];
const int dimCliente {15};

//Esta funcion se utiliza para deserializar los datos de un archivo binario
string readstring(fstream& archi, int dim){
    buffer[dim]='\0';
    archi.read(buffer, dim);
    return buffer;
}

//sobrecarga dek operador para la estrucutra producto
fstream& operator >>(fstream& archivo, producto& prod){
    prod.SKU= readstring(archivo, dimSKU);
    prod.descripcion= readstring(archivo, dimdescr);
    archivo.read(reinterpret_cast<char *>(&prod.costofijo), sizeof(prod.costofijo));
    return archivo;
}

//sobrecarga dek operador para la estrucutra reparaciones
fstream& operator >>(fstream& archivo, reparaciones& repa){//sobrecarga dek operador para la estrucutra producto
    repa.cliente = readstring(archivo, dimCliente);
    archivo.read(reinterpret_cast<char *>(&repa.tiproduct), sizeof(repa.tiproduct));
    repa.SKU= readstring(archivo, dimSKU);
    archivo.read(reinterpret_cast<char *>(&repa.costoDirecto), sizeof(repa.costoDirecto));
    archivo.read(reinterpret_cast<char *>(&repa.presupuestado), sizeof(repa.presupuestado));
    return archivo;
}

//Estas funciones son para mostrar las distintas estructuras
//Funcion para producto
void mostrar(producto v[], int dim){
    for(int i=0; i<dim; i++){
        cout<<"SKU: "<<v[i].SKU<<"| DESCRIP: "<<v[i].descripcion<<"| costo fijo: "<<v[i].costofijo<<endl;
    }
}

//Funcion para Reparaciones
void mostrar(reparaciones v[], int dim){
    for(int i=0; i<dim; i++){
        cout<<"CLIENTE: "<<v[i].cliente<<"| TIPO DE PRODUCTO: "<<v[i].tiproduct<<"| SKU: "<<v[i].SKU<<"|COSTO DIRECTO: "<<v[i].costoDirecto<<"|PRESUPUESTADO "<<v[i].presupuestado<<endl;
    }
}

/*
    Esta función se utiliza para obtener la cantidad de registros del archivo "reparaciones.bin", ya que este varia en cada ejecución
    Por parametro se pide el archivo y el tamaño de cada registro (previamente calculado), luego calcula el tamaño del archivo en bytes
    para eso va a la ultima poscicion del archivo y pide el valor de esa posición, luego se divide la longuitud del archivo por el 
    tamaño de los registros y se retorna.
*/
int obtenerDimension(fstream& archivo, int tamañoRegistro){
    int longitud;
    archivo.seekg(0, ios::end);
    longitud = archivo.tellg(); 
    archivo.seekg(0, ios::beg);

    return longitud/tamañoRegistro;
}

//Funcion de ordenar por incersión
template <typename T> void ordenar(T vec[], int dim, int (*criterio)(T, T))
{
	int i, j;
	T aux;

	for (i=1 ; i < dim ; i++) {
		aux = vec[i];
		j = i-1;

		//&& vec[j] > aux
		while (j >= 0 && criterio(vec[j], aux) > 0) {
			vec[j + 1] = vec[j];
			j--;
		}
		vec[j+1] = aux;
	}
}

//Criterio para ordenar el vector reparaciones por cliente, luego por tipo de producto y por ultimo por SKU
int criterio_ordenar(reparaciones a, reparaciones b)
{
	if ((a.cliente).compare(b.cliente) == 0){
        if (a.tiproduct == b.tiproduct){
            return a.SKU < b.SKU ? -1 : 1;
        }
        else{
            return a.tiproduct < b.tiproduct ? -1 : 1;
        }
    }

    else{
		return a.cliente.compare(b.cliente);
    }
}

//Criterio para ordenar por cliente
int criterio_cliente(reparaciones a, string b){
	return a.cliente.compare(b);
}

//Esta funcion es para buscar de manera lineal el SKU que coincide con el de la reparacion y poder proporcionar su nombre
int buscar_lineal_ord(string clave, producto vec[], int dim)
{
	int i;
	for (i = 0; i < dim && (vec[i].SKU.compare(clave)) < 0; i++);
	return (i < dim && (vec[i].SKU.compare(clave))) == 0 ? i : -1; 
}

//Esta funcion busca en la lista repacraciones la estructura que corresponde al cliente y muestra los datos, tambien cruza los datos con los vectores Producto y Tipo
template <typename T> int buscar_lineal_ord_lista(T vec[], producto vecProd[], string vecTipo[], int dim, int (*criterio)(T,string), string clave, int dimProd)
{
	int i;
    //Cantidad de coincidencias, sirve para dar error de cliente no encontrado
    int x = 0; 
    //Guarda la posicion encontrada por buscar_lineal_ord
    int posProduct = 0;
    float ganancia = 0;
	for (i = 0; i < dim && criterio(vec[i], clave) <= 0; i++){
        if(criterio(vec[i], clave) == 0){
            posProduct = buscar_lineal_ord(vec[i].SKU, vecProd, dimProd);
            
            //Muesta los datos
            cout
            << "CLIENTE: "<<vec[i].cliente 
            <<"|TIPO DE PRODUCTO: "<<vecTipo[vec[i].tiproduct]
            <<"|SKU: "<<vec[i].SKU
            <<"|PRODUCTO: "<<vecProd[posProduct].descripcion
            <<"|COSTO FIJO: "<<vecProd[posProduct].costofijo
            <<"|COSTO DIRECTO: "<<vec[i].costoDirecto
            <<"|PRESUPUESTADO: "<<vec[i].presupuestado<<endl;

            x++;
            //Se acumula la ganancia de cada registro encontrado, al finalizar el bucle se muestra la cantidad de reparaciones
            ganancia += vec[i].presupuestado - (vecProd[posProduct].costofijo + vec[i].costoDirecto);
        }
    }
    if(x != 0){
        cout<<"La ganancia con el cliente "<<clave<<" es: "<<ganancia<<endl;
    }

    return x;
}

int main(){
    const int dimprod {10};//Dimension del vector producto, la cual es fija
    string vectorTipo[] = {"Electronico","Mecanico","Mecatronico"};
    producto vectorProductos[dimprod]={};
    fstream archivoproductos;
    archivoproductos.open("productos.bin", ios::binary|ios::in);
    if(!archivoproductos){
        cout<<"no se pudo abrir el archivoproductos"<<endl;
        return 1;
    }
    //Bucle para cargar los datos del archivo al vector
    for(int i=0; i<dimprod; i++){
        archivoproductos>>vectorProductos[i];
    }
    archivoproductos.close();
    mostrar(vectorProductos, dimprod);

    fstream archivoReparaciones;
    archivoReparaciones.open("reparaciones.bin", ios::binary|ios::in);
    if(!archivoReparaciones){
        cout<<"no se pudo abrir el archivoproductos"<<endl;
        return 1;
    }

    //Aca se calcula el de cada registro, para eso se suma los bytes que ocupa cada variable, y estos siempre son fijos
    const int tamañoRegistro = 2*(sizeof(float)) + (sizeof(int)) + dimSKU + dimCliente;
    const int cantidadRegistros = obtenerDimension(archivoReparaciones, tamañoRegistro);
    //Este es un vector dinamico, este se crea mediante un puntero
    reparaciones *vectorReparaciones = new reparaciones[cantidadRegistros];

    for(int i=0; i<cantidadRegistros; i++){
        archivoReparaciones>> vectorReparaciones[i];
    }
    archivoReparaciones.close();

    //Se ordena el vector reparaciones
    ordenar(vectorReparaciones, cantidadRegistros, criterio_ordenar);
    mostrar(vectorReparaciones, cantidadRegistros);

    int x;
    //Aca se busca a un cliente y se calcula la gananvia
    string entrada;
    do{
        cout<<"Ingrese el nombre del cliente: "<<endl;
        if(!(getline(cin, entrada))){
            break;
        }
        x = buscar_lineal_ord_lista(vectorReparaciones, vectorProductos, vectorTipo, cantidadRegistros, criterio_cliente, entrada, dimprod);
        if(x == 0){
            cout<<"No se encontro ese cliente"<<endl;
        }
    }while(true);

    //Esto libera el vector dinamico que generamos
    delete[] vectorReparaciones;

    return 0;
}