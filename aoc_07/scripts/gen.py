from itertools import permutations 


perms = list(permutations(range(5)))
for perm in perms:
    for el in perm:
        print(el) 

