# Testy na vstavane funkcie

Doporucene sledovat log pre spolahlivejsi prehlad o stave.


## test_defined

Spustit siet a poslat na input `in` nejaky vstupny retazec.

**Ocakavane stavy**

- premenna `var` bude nastavena na hodnotu `2`.


## test_elapsed_place

Spustit siet a poslat na input `in` nejaky vstupny retazec.

**Ocakavane stavy**

- premenna `time_end` bude nastavena na nejake velke cislo.


## test_elapsed_transition

Spustit siet a poslat na input `in` nejaky vstupny retazec.

**Ocakavane stavy**

- premenna `time_now` bude nastavena na vysoke cislo.


## test_output

Spustit siet.

**Ocakavane stavy**

- pri odpaleni `int_literal` ma byt na vstupe `out` cislo `27`.
- pri odpaleni `float_literal` ma byt na vstupe `out` cislo `3.6`.
- pri odpaleni `str_literal` ma byt na vstupe `out` retazec `test`.
- pri odpaleni `char_literal` ma byt na vstupe `out` cislo `99` (c).
- pri odpaleni `var_int` ma byt na vstupe `out` cislo `858`.
- pri odpaleni `var_float` ma byt na vstupe `out` cislo `3.1415`.
- pri odpaleni `var_char` ma byt na vstupe `out` cislo `65` (A).


## test_tokens

Spustit siet a poslat na input `in` nejaky vstupny retazec.

**Ocakavane stavy**

- premenna `var` bude nastavena na aktualny pocet tokenov.


## test_valueof

Spustit siet a poslat na input `in` nejake cislo.

**Ocakavane stavy**

- premenna `var` bude nastavena na cislo zadane na vstupe `in`.
