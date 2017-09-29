# Usage:
# python xor-multidevice.py --dynet-devices CPU,GPU:0
# or python xor-multidevice.py --dynet-gpus 1
# or python xor-mutltidevice.py --dynet-gpu

import sys 
import dynet as dy


#xsent = True
xsent = False

HIDDEN_SIZE = 8 
ITERATIONS = 2000

m = dy.Model()
trainer = dy.SimpleSGDTrainer(m)

pW = m.add_parameters((HIDDEN_SIZE, 2)) # GPU:0 by default 
pb = m.add_parameters(HIDDEN_SIZE, device="GPU:0")
pV = m.add_parameters((1, HIDDEN_SIZE), device="CPU")
pa = m.add_parameters(1, device="CPU")

if len(sys.argv) == 2:
  m.populate_from_textfile(sys.argv[1])

W = dy.parameter(pW)
b = dy.parameter(pb)
V = dy.parameter(pV)
a = dy.parameter(pa)

x = dy.vecInput(2, "GPU:0")
y = dy.scalarInput(0, "CPU")
h = dy.tanh((W*x) + b)
h_cpu = dy.to_device(h, "CPU")
if xsent:
    y_pred = dy.logistic((V*h_cpu) + a)
    loss = dy.binary_log_loss(y_pred, y)
    T = 1 
    F = 0 
else:
    y_pred = (V*h_cpu) + a 
    loss = dy.squared_distance(y_pred, y)
    T = 1 
    F = -1


for iter in range(ITERATIONS):
    mloss = 0.0 
    for mi in range(4):
        x1 = mi % 2 
        x2 = (mi // 2) % 2 
        x.set([T if x1 else F, T if x2 else F]) 
        y.set(T if x1 != x2 else F)
        mloss += loss.scalar_value()
        loss.backward()
        trainer.update()
    mloss /= 4.
    print("loss: %0.9f" % mloss)

x.set([F,T])
z = -(-y_pred)
print(z.scalar_value())

m.save("xor.pymodel")

dy.renew_cg()
W = dy.parameter(pW)
b = dy.parameter(pb)
V = dy.parameter(pV)
a = dy.parameter(pa)

x = dy.vecInput(2)
y = dy.scalarInput(0)
h = dy.tanh((W*x) + b)
h_cpu = dy.to_device(h, "CPU")
if xsent:
    y_pred = dy.logistic((V*h_cpu) + a)
else:
    y_pred = (V*h_cpu) + a 
x.set([T,F])
print("TF",y_pred.scalar_value())
x.set([F,F])
print("FF",y_pred.scalar_value())
x.set([T,T])
print("TT",y_pred.scalar_value())
x.set([F,T])
print("FT",y_pred.scalar_value())
