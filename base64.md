## Polyglots with hidden base64 messages

1. First I have to know what message or code I want to embed in the image header, for example:
    ```
    Hi Alice, the password is: 5agS[z'm
    ```

2. I encode the message in `base64`:
    ```bash
    ~$ echo "Hi Alice, the password is: 5agS[z'm" | base64
    SGkgQWxpY2UsIHRoZSBwYXNzd29yZCBpczogNWFnU1t6J20K
    ```

3. I calculate the length of the already encoded message, because each character will be a new byte in the header and I need to know the number of bytes with which I am going to work.
    ```
    ~$ expr length "SGkgQWxpY2UsIHRoZSBwYXNzd29yZCBpczogNWFnU1t6J20K"
    48
    ```

4. To this number of bytes we will have to add the `16` bytes (`0x10`) of which the header of a JPEG file is already composed. So, `16` + `48` = `64`, and in hexadecimal it is `0x40`.
    ```bash
    ~$ printf "%x\n" "64"
    40
    ```

5. `64` is a number of bytes greater than `23`, and I need to indicate in offset `0x5` the byte `23` so that the header starts with a comment, otherwise it will not work. So I use the two bytes `0123`, which is a valid header size and the second bytes is `23`, which represents the character of the `#` pad. First modify this 2 bytes `0010` that indicate the size of the message. To do this press `C` to activate the cursor and to scroll through each byte. You need to press `i` to activate insert mode. Put the cursor on the offset `0x4`. Change the byte `00` to `01` and byte `10` from offset `0x5` to `23`, which is the hexadecimal value that represents the ASCII character for `#` pad.
    ```
    [0x00000000 + 5> * INSERT MODE *
    - offset - | 0 1  2 3  4 5  6 7  8 9  A B  C D  E F| 0123456789ABCDEF  comment
    0x00000000 |ffd8 ffe0 0123 4a46 4946 0001 0101 0048| .....#JFIF.....H
    0x00000010 |0048 0000 ffdb 0043 0006 0405 0605 0406| .H.....C........
    ```

    The purpose of modifying this byte is, on the one hand, to extend the length of the header an additional number of bytes and, on the other hand, to indicate that the code below to fill the specified size is a comment.

6. Now calculate again the difference between the original image size (`0x0010`) and the new size (`0x0123`). First with `rax2` I calculate the number of bytes that `0x0010` represents:
    ```bash
    ~$ rax2 0x0010
    16
    ```
    Then I calculate the bytes that the new value `0x0123` represents:
    ```bash
    ~$ rax2 0x0123
    291
    ```
    Now I calculate the difference between `35` and` 16`, and the result is `8`:
    ```bash
    ~$ echo "291-16" | bc
    275
    ```

7. Well, `275` is the number of bytes to embed from offset `0x14`. To do this I open `Radare2` again in write mode:
    ```bash
    ~$ r2 -w dog.jpg
    ```
    We go into *write extend* mode by typing `weN` to insert bytes at some address, and pressing enter.
    ```bash
    [0x00000000]> weN 0x14 275
    ```
    Where `0x14` (row `0x10`, column `4`, value `ff`) is the offset or address where I will insert the new bytes, and `275` is the number of bytes to insert.

8. Enter in visual mode again with `V` and pressing enter.
    ```
    [0x00000014]> V
    ```
    Can scroll up to the top with the mouse scroll and see the start of the file from the first offset. It looks like this:
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
    As you can see, hems successfully added 275 bytes with value `00` from offset `0x14` to offset `0x126` (row `0x120`, column `6`, value `00`), shifting all other bytes forward.

9. Now is the time to overwrite some of the bytes that I have added by pressing `i` to enter INSERT mode. For example, enter a carriage return (CR) by `0d` at offset `0x12` and line feed (LF) by `0a` at offset `0x13`:
    ```
    [0x00000000 + 20> * INSERT MODE *
    - offset - | 0 1  2 3  4 5  6 7  8 9  A B  C D  E F| 0123456789ABCDEF  comment
    0x00000000 |ffd8 ffe0 0123 4a46 4946 0001 0101 0048| .....#JFIF.....H
    0x00000010 |0048 0d0a 0000 0000 0000 0000 0000 0000| .H..............
    ...
    ```
    **Note**: *The concepts of line feed (`LF`) and carriage return (`CR`) are closely associated, and can be considered either separately or together. Although the design of a machine (typewriter or printer) must consider them separately, the abstract logic of software can combine them together as one event. This is why a newline in character encoding can be defined as `LF` and `CR` combined into one (commonly called `CR`+`LF` or `CRLF`).*

10. The next bytes from offset `0x14` are bytes that I can replace by adding text. For example, I press the tab to change columns and stay where the ASCII representation of the bytes is displayed. Now write ASCII characters directly, not bytes, for example `ls -lrt; exit;`.
    ```
    [0x00000000 + 68> * INSERT MODE *
    - offset -   0 1  2 3  4 5  6 7  8 9  A B  C D |0123456789ABCD| comment
    0x00000000  ffd8 ffe0 0123 4a46 4946 0001 0101 |.....#JFIF....|
    0x0000000e  0048 0048 0d0a 5347 6b67 5157 7870 |.H.H..SGkgQWxp|
    0x0000001c  5932 5573 4948 526f 5a53 4277 5958 |Y2UsIHRoZSBwYX|
    0x0000002a  4e7a 6432 3979 5a43 4270 637a 6f67 |Nzd29yZCBpczog|
    0x00000046  0000 0000 0000 0000 0000 0000 0000 |..............|
    ...
    ```
I change the column again by pressing the tabulator and stay again in the column of the bytes. To exit press `Esc` twice, type `q` (quit), retype `q` again and press enter.

11. It's the moment to open the photo with a different program from the one we have configured by default to view photographs, for example with `cat` and passing the output to the executable `sh`, `bash` or the *shell* program of your choice.
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
    An error on the screen, ok, but in this error there is information that could be of interest to Alice.

12. Now Alice can decode the message that appears on the screen with base64 and read the original secret message.
    ```bash
    ~$ echo "SGkgQWxpY2UsIHRoZSBwYXNzd29yZCBpczogNWFnU1t6J20K" | base64 -d
    Hi Alice, the password is: 5agS[z'm
    ```
