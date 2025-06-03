from random import randint

# Cria um arquivo de 8192 bytes (32 páginas de 256 bytes)
with open("data_memory.txt", "wb") as f:
    for _ in range(8192):
        f.write(bytes([randint(0, 255)]))  # cada valor entre 0-255 ocupa 1 byte

# Mesmo conteúdo para a backing store
with open("backing_store.txt", "wb") as f:
    for _ in range(8192):
        f.write(bytes([randint(0, 255)]))
