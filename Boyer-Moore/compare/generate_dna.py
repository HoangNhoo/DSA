import random

# Define the possible nucleotides
nucleotides = ['A', 'T', 'C', 'G']

# Generate a random DNA sequence of length 10,000
dna_sequence = ''.join(random.choices(nucleotides, k=100))

# Save the sequence to a .txt file
with open("dna_sequence.txt", "w") as file:
    file.write(dna_sequence)

print("DNA sequence saved as 'dna_sequence.txt'")
