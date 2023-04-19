#!/bin/bash

# Definir variables de entorno para el cliente
export IP_TUPLAS=localhost
export PORT_TUPLAS=8080

# Inicializar el servidor en el puerto 8080
./servidor 8080 &

# Esperar a que el servidor esté listo antes de continuar
sleep 1

# Insertar una tupla en el servidor
./cliente set_value 1 "valor1" 2 3.14

# Obtener los valores de la tupla insertada
./cliente get_value 1

# Modificar los valores de la tupla insertada
./cliente modify_value 1 "nuevo valor1" 3 2.71

# Obtener los valores de la tupla modificada
./cliente get_value 1

# Borrar la tupla insertada
./cliente delete_key 1

# Comprobar si la tupla ha sido borrada
./cliente exist 1

# Insertar una tupla y copiarla a otra clave
./cliente set_value 1 "valor1" 2 3.14
./cliente copy_key 1 2

# Obtener los valores de la tupla original
./cliente get_value 1

# Obtener los valores de la tupla copiada
./cliente get_value 2

# Limpiar el servidor
./cliente init

# Cerrar el servidor
pkill servidor


# Definir variables de entorno para el cliente
export IP_TUPLAS=localhost
export PORT_TUPLAS=5000

# Inicializar el servicio
echo "Iniciando servicio..."
./servidor 5000 &
sleep 1

# Prueba de inserción de elementos
echo "Insertando elementos..."
./cliente insert 1 "valor1" 10 3.14
./cliente insert 2 "valor2" 20 6.28
./cliente insert 3 "valor3" 30 9.42
./cliente insert 4 "valor4" 40 12.56
./cliente insert 1 "valor5" 50 15.70 # debería devolver error

# Prueba de obtención de elementos
echo "Obteniendo elementos..."
./cliente get 1
./cliente get 2
./cliente get 5 # debería devolver error

# Prueba de modificación de elementos
echo "Modificando elementos..."
./cliente modify 3 "nuevo_valor3" 300 90.42
./cliente get 3
./cliente modify 5 "valor5" 50 15.70 # debería devolver error

# Prueba de eliminación de elementos
echo "Eliminando elementos..."
./cliente delete 2
./cliente delete 5 # debería devolver error

# Prueba de existencia de elementos
echo "Comprobando existencia de elementos..."
./cliente exist 1
./cliente exist 2 # debería devolver error

# Prueba de copia de elementos
echo "Copiando elementos..."
./cliente copy 1 2
./cliente get 2
./cliente copy 5 6 # debería devolver error

# Detener el servicio
echo "Deteniendo servicio..."
killall servidor

echo "Pruebas completadas."