This repository is used to generate benchmarks for maskVerif from the `.nl` file given by SILVER.
# Example Usage
```bash
./bin/Benchmarks --num_secret 8 --num_output 8 --num_inshares 2 --num_outshares 2 \
--num_ref 38 --step_in 8 --step_out 8 --insfile  nl/aes/aes_sbox_dep1.nl --order 1
```
If the commandline options are not set properly, the program will break. So set the options carefully.
