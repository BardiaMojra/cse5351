import numpy as np
import matplotlib
from matplotlib import pyplot as plt

from nbug import *
from pdb import set_trace as st



''' matplotlib config '''
matplotlib.pyplot.ion()
plt.style.use('ggplot')

primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97,
  101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199,
	211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293,
	307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397,
 	401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499,
503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599,
	601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691,
 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797,
809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887,
907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997 ]


p_min = 1 # min processors
p_max = 32 # max processors
ps = [p for p in range(p_min,p_max+1)]
x = 1 # unit striking time
n = 1000000 # prime number smaller than n
k = len(primes)
lamb = 500 * x # unit communication time
speedups = list()
for p in ps:
  total_strike_time = 0.0
  for prime in primes:
    total_strike_time += (n/p) / prime
  # total_strike_time = int(total_strike_time * x)
  total_strike_time = (total_strike_time * x)
  total_comm_time = k*(p-1) * lamb
  total_time = total_comm_time + total_strike_time
  print(f'processors: {p}')
  print(f'total_strike_time: {total_strike_time}')
  print(f'total_comm_time: {total_comm_time}')
  print(f'total_time: {total_time}')
  log = np.asarray([p, total_time])
  speedups.append(log)

speedups = np.asarray(speedups).reshape(-1,2)
speedups[:,1] = speedups[0,1]/speedups[:,1]
nprint('speedups', speedups)
plt.title("Lamba = 500X")
plt.xlabel("number of processors")
plt.ylabel("speedup")
plt.plot(speedups[:,0], speedups[:,1])
plt.show()
st()

# EOF
