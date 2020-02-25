import re
import sys

def mod_power(base, x, n):
    # return (x ** n) % base

    if n == 0:
        return 1

    accum = mod_power(base, x, n // 2)
    accum *= accum
    if n%2 == 1:
        accum *= x
    accum = accum % base
    return accum

def mod_inv(base, val):
    if val < 0:
        val += base

    r = [base, val]
    q = [0, 0]

    n = 1
    while r[n] != 0:
        n += 1
        next_q = r[n-2] // r[n-1]
        next_r = r[n-2] % r[n-1]

        r.append(next_r)
        q.append(next_q)

    n -= 1

    if r[n] != 1:
        import pdb; pdb.set_trace()
        print(f'Not coprime {base} {val} {r[n]}')

    w = 1
    v = 0

    # Backtrack
    for i in range(n-1):
        temp = v
        v = w
        w = temp - q[n-i] * w
        w %= base

    check = (w * val) % base
    if check != 1 and check + base != 1:
        print(f'Failure of inv: {base} {val} {w}')

    return w

class Shuffle:
    def __init__(self, base):
        self.base = base
        self.m = 1
        self.b = 0

    def at(self, x):
        return (self.m * x + self.b) % self.base

    def fixup(self):
        self.m %= self.base
        self.b %= self.base

    def __str__(self):
        return f'y = {self.m}x {self.b:+} mod {self.base}'

    def inv(self, y):
        minv = mod_inv(self.base, self.m)
        return (minv * (y - self.b)) % self.base

    def invert(self):
        self.m = mod_inv(self.base, self.m)
        self.b = -self.m * self.b
        self.fixup()

    def cut(self, n, p=False, inv=False):
        if p:
            print(f'CUT {n} {"inv" if inv else ""}')
        if inv:
            self.b -= self.m * n
        else:
            self.b += self.m * n
        self.fixup()

    def new_stack(self, p=False, inv=False):
        if p:
            print(f'STCK {"inv" if inv else ""}')
        m = self.m
        self.m *= -1
        self.b = self.b - m
        self.fixup()

    def incr(self, n, p=False, inv=False):
        if p:
            print(f'INCR: {n} {"inv" if inv else ""}')
        if inv:
            self.m *= n
        else:
            self.m *= mod_inv(self.base, n)
        self.fixup()

if __name__ == '__main__':
    cut_re = re.compile(r'cut ([\-0-9]+)')
    new_re = re.compile(r'deal into new stack')
    incr_re = re.compile(r'deal with increment ([\-0-9]+)')
    with open(sys.argv[1], 'r') as file:
        lines = list(file)

    ops = []
    for line in lines:
        cut_res = cut_re.search(line)
        new_res = new_re.search(line)
        incr_res = incr_re.search(line)
        if cut_res is not None:
            ops.append((Shuffle.cut, int(cut_res.group(1))))
        elif new_res is not None:
            ops.append((Shuffle.new_stack, None))
        elif incr_res is not None:
            ops.append((Shuffle.incr, int(incr_res.group(1))))

    shuff = Shuffle(10007)
    for func, arg in ops:
        if arg is not None:
            func(shuff, arg)
        else:
            func(shuff)

    print(shuff)
    print(f'Find 2019 = {shuff.inv(2019)}')
    shuff.invert()
    print(shuff)
    print(f'Find 2019 = {shuff.at(2019)}')

    num_cards = 119315717514047
    num_rounds = 101741582076661
    shuff = Shuffle(num_cards)
    for func, arg in ops:
        if arg is not None:
            func(shuff, arg)
        else:
            func(shuff)

    shuff_back = Shuffle(num_cards)
    shuff_back.m = shuff.m
    shuff_back.b = shuff.b
    # shuff.invert()
    print(shuff)

    m_to_the_n = mod_power(shuff.base, shuff.m, num_rounds)
    m_minus_one_inv = mod_inv(num_cards, shuff.m - 1)

    # THIS FUCKING MINUS 1!!!!
    shuff.b = m_minus_one_inv * (m_to_the_n - 1) * shuff.b
    shuff.m = m_to_the_n
    shuff.fixup()
    print(shuff)

    # import pdb; pdb.set_trace()
    print(f'Do it 2019: {shuff.at(2020)}')


