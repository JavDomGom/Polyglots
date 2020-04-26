## Polyglots con mensajes ocultos en base64

1. Primero tengo que saber qué mensaje o código quiero insertar en el encabezado de la imagen, por ejemplo:
    ```
    Hi Alice, the password is: 5agS[z'm
    ```

2. Codificamos este mensaje en `base64`:
    ```bash
    ~$ echo "Hi Alice, the password is: 5agS[z'm" | base64
    SGkgQWxpY2UsIHRoZSBwYXNzd29yZCBpczogNWFnU1t6J20K
    ```

3. Calculamos la longitud del mensaje ya codificado, ya que cada carácter será un nuevo byte en el encabezado y necesitaremos conocer la cantidad de bytes con los que vamos a trabajar.
    ```
    ~$ expr length "SGkgQWxpY2UsIHRoZSBwYXNzd29yZCBpczogNWFnU1t6J20K"
    48
    ```

4. A este número de bytes tendremos que agregar los `16` bytes (`0x10`) de los que inicialmente se compone el encabezado de un archivo JPEG. Así pues, `16`+`48`=`64`, que en hexadecimal es `0x40`.
    ```bash
    ~$ printf "%x\n" "64"
    40
    ```

5. `64` es un número de bytes mayor que `23`, y necesitamos indicar sí o sí en el *offset* `0x5` el byte `23` para indicar que el encabezado comienza con un comentario, de lo contrario no funcionará. Para solucionar esto usaremos los dos bytes `0123`, que es un tamaño de encabezado válido y el segundo byte es `23`, que representa el carácter de la almohadilla `#`. Primero modificaremos los 2 bytes `0010` que indican el tamaño del mensaje. Para hacer esto, presionamos `C` para activar el cursor y desplazarnos por cada byte de la pantalla. Ahora presionamos `i` para activar el modo INSERT. Colocamos el cursor en el *offset* `0x4`. Cambie el byte `00` a `01` y el byte `10` del *offset* `0x5` a `23`, que es el valor hexadecimal que representa el carácter ASCII para la almohadilla `# `.
    ```
    [0x00000000 + 5> * INSERT MODE *
    - offset - | 0 1  2 3  4 5  6 7  8 9  A B  C D  E F| 0123456789ABCDEF  comment
    0x00000000 |ffd8 ffe0 0123 4a46 4946 0001 0101 0048| .....#JFIF.....H
    0x00000010 |0048 0000 ffdb 0043 0006 0405 0605 0406| .H.....C........
    ```

    El propósito de modificar este byte es, por un lado, extender la longitud del encabezado un número adicional de bytes y, por otro lado, indicar que los datos que usaremos para llenar el tamaño especificado son un comentario.

6. Ahora calculamos la diferencia entre el tamaño original (`0x0010`) y el nuevo tamaño (`0x0123`). Primero calculamos con `rax2` el número de bytes que representa `0x0010`:
    ```bash
    ~$ rax2 0x0010
    16
    ```
    Luego calculamos los bytes que representa el nuevo valor `0x0123`:
    ```bash
    ~$ rax2 0x0123
    291
    ```
    Finalmente calculamos la diferencia entre `35` y `16`, y el resultado es `275`:
    ```bash
    ~$ echo "291-16" | bc
    275
    ```

7. Bueno, `275` es el número de bytes para incrustar desde el *offset* `0x14`. Para hacer esto, utilizamos `Radare2` nuevamente en modo de escritura:
    ```bash
    ~$ r2 -w dog.jpg
    ```
    Entramos en el modo *write extend* escribiendo `weN` para insertar una cantidad de bytes a partir de la dirección que indiquemos.
    ```bash
    [0x00000000]> weN 0x14 275
    ```
    Donde `0x14` es el *offset* o la dirección donde insertaremos los nuevos bytes, y `275` es el número de bytes a insertar.

8. Entramos de nuevo en modo visual con `V` y presionamos intro.
    ```
    [0x00000014]> V
    ```
    Podemos desplazarnos hasta la parte superior con el scroll y ver el inicio del archivo desde el primer *offset*. Se veremos algo como esto:
    ```
    [0x00000000 0% 2352 dog_b64.jpg]> xc
    - offset -   0 1  2 3  4 5  6 7  8 9  A B  C D  E F  0123456789ABCDEF  comment
    0x00000000  ffd8 ffe0 0123 4a46 4946 0001 0101 0048  .....#JFIF.....H
    0x00000010  0048 0000 0000 0000 0000 0000 0000 0000  .H..............
    0x00000020  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x00000030  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x00000040  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x00000050  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x00000060  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x00000070  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x00000080  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x00000090  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x000000a0  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x000000b0  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x000000c0  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x000000d0  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x000000e0  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x000000f0  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x00000100  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x00000110  0000 0000 0000 0000 0000 0000 0000 0000  ................
    0x00000120  0000 0000 0000 00ff db00 4300 0604 0506  ..........C.....
    ...
    ```
    Como se puede ver, se han agregado con éxito 19 bytes con el valor `00` desde el *offset* `0x14` hasta el *offset* `0x126`, desplazando todos los demás bytes hacia adelante.

9. Ahora es el momento de sobrescribir algunos de los bytes que acabamos de agregar presionando `i` para entrar en modo INSERT. Por ejemplo, añadimos un retorno de carro (CR) con el valor `0d` en el *offset* `0x12` y un salto de línea (LF) con el valor `0a` en el *offset* `0x13`:
    ```
    [0x00000000 + 20> * INSERT MODE *
    - offset - | 0 1  2 3  4 5  6 7  8 9  A B  C D  E F| 0123456789ABCDEF  comment
    0x00000000 |ffd8 ffe0 0123 4a46 4946 0001 0101 0048| .....#JFIF.....H
    0x00000010 |0048 0d0a 0000 0000 0000 0000 0000 0000| .H..............
    ...
    ```
    **Nota**: *El concepto de avance de línea (CF) y retorno de carro (CR) están estrechamente asociados y pueden considerarse por separado o en conjunto. Así pues una nueva línea en la codificación de caracteres se puede definir como `LF` y `CR` combinadas (comúnmente `CR`+`LF` o `CRLF`).*

10. Los siguientes bytes a partir del *offset* `0x14` son bytes que podemos reemplazar agregando texto. Por ejemplo, presionamos el tabulador para cambiar de columna y posicionarnos donde se muestra la representación ASCII de los bytes. Ahora podemos escribir caracteres ASCII directamente, no bytes, por ejemplo `SGkgQWxpY2UsIHRoZSBwYXNzd29yZCBpczogNWFnU1t6J20K`.
    ```
    [0x00000000 + 68> * INSERT MODE *
    - offset -   0 1  2 3  4 5  6 7  8 9  A B  C D  E F |0123456789ABCDEF| comment
    0x00000000  ffd8 ffe0 0123 4a46 4946 0001 0101 0048 |.....#JFIF.....H|
    0x00000010  0048 0d0a 5347 6b67 5157 7870 5932 5573 |.H..SGkgQWxpY2Us|
    0x00000020  4948 526f 5a53 4277 5958 4e7a 6432 3979 |IHRoZSBwYXNzd29y|
    0x00000030  5a43 4270 637a 6f67 4e57 466e 5531 7436 |ZCBpczogNWFnU1t6|
    0x00000040  4a32 304b 0000 0000 0000 0000 0000 0000 |J20K............|
    ...
    ```
Vuelvo a cambiar la columna presionando el tabulador de nuevo y nos posicionamos de nuevo en la columna donde se ven los bytes. Para salir, presionamos `Esc` dos veces, escribimos `q` (quit) un par de veces y pulsamos Intro.

11. Es el momento de abrir la foto con un programa diferente al que hemos configurado de forma predeterminada para ver fotografías, por ejemplo con `cat`, y pasaremos la salida estándar al programa ejecutable `sh`, `bash` o *shell* que queramos.
    ```bash
    $ cat dog_b64.jpg | sh
    : not found#JFIFHH
    sh: 2: SGkgQWxpY2UsIHRoZSBwYXNzd29yZCBpczogNWFnU1t6J20K��C: not found
    sh: 3: : not found
    sh: 6: 
            %#: not found
    sh: 7:: not found
    sh: 8: : not found
    sh: 10: Syntax error: word unexpected (expecting ")")
    ```
    Aparece un error en la pantalla, ok, pero en este error hay información que podría ser de interés para Alice.

12. Ahora Alice puede decodificar el mensaje que aparece en la pantalla con base64 y leer el mensaje secreto original.
    ```bash
    ~$ echo "SGkgQWxpY2UsIHRoZSBwYXNzd29yZCBpczogNWFnU1t6J20K" | base64 -d
    Hi Alice, the password is: 5agS[z'm
    ```
