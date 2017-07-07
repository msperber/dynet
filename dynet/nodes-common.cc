#include "dynet/nodes.h"

#include <limits>
#include <cmath>
#include <sstream>

#include "dynet/nodes-macros.h"
#include "dynet/globals.h"

using namespace std;

namespace dynet {

string AddVectorToAllColumns::as_string(const vector<string>& arg_names) const {
  ostringstream os;
  os << "colwise_add(" << arg_names[0] << ", " << arg_names[1] << ')';
  return os.str();
}

Dim AddVectorToAllColumns::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2 &&
                          xs[0].rows() == xs[1].rows() &&
                          xs[0].ndims() == 2 &&
                          (xs[1].ndims() == 1 || (xs[1].ndims() == 2 && xs[1].cols() == 1)),
                          "Bad input dimensions in AddVectorToAllColumns: " << xs);
  return Dim({xs[0][0], xs[0][1]}, max(xs[0].bd,xs[1].bd));
}

string SparsemaxLoss::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "sparsemax(" << arg_names[0] << ", q)";
  return s.str();
}

Dim SparsemaxLoss::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1 && LooksLikeVector(xs[0]), "Bad input dimensions in SparsemaxLoss: " << xs);
  return Dim({1});
}

string Sparsemax::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "sparsemax(" << arg_names[0] << ")";
  return s.str();
}

Dim Sparsemax::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1 && LooksLikeVector(xs[0]), "Bad input dimensions in Sparsemax: " << xs);
  return xs[0];
}

string MatrixInverse::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "inverse(" << arg_names[0] << ")";
  return s.str();
}

Dim MatrixInverse::dim_forward(const vector<Dim>& xs) const {
  return xs[0];
}

string LogDet::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "logdet(" << arg_names[0] << ")";
  return s.str();
}

Dim LogDet::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs[0].ndims() <= 2 && (xs[0].rows() == xs[0].cols()), "Bad arguments in LogDet: " << xs);
  return Dim({1});
}

string SelectRows::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "select_rows(" << arg_names[0] << ", {rsize=" << prows->size() << "})";
  return s.str();
}

Dim SelectRows::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Bad arguments in SelectRows: " << xs);
  unsigned nrows = prows->size();
  Dim ret(xs[0]);
  ret.d[0] = nrows;
  return ret;
}

string SelectCols::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "select_cols(" << arg_names[0] << ", {csize=" << pcols->size() << "})";
  return s.str();
}

Dim SelectCols::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1 && xs[0].ndims() == 2, "Bad arguments in SelectCols: " << xs);
  unsigned ncols = pcols->size();
  return Dim({xs[0].rows(), ncols});
}

string Min::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "min{" << arg_names[0] << ", " << arg_names[1] << "}";
  return s.str();
}

Dim Min::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2 && xs[0] == xs[1], "Bad arguments in Min: " << xs);
  return xs[0].bd >= xs[1].bd ? xs[0] : xs[1];
}

string Max::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "max{" << arg_names[0] << ", " << arg_names[1] << "}";
  return s.str();
}

Dim Max::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2 && xs[0] == xs[1], "Bad arguments in Max: " << xs);
  return xs[0].bd >= xs[1].bd ? xs[0] : xs[1];
}

string TraceOfProduct::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "Tr(" << arg_names[0] << " * " << arg_names[1] << "^T)";
  return s.str();
}

Dim TraceOfProduct::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2 && xs[0] == xs[1], "Bad arguments in TraceOfProduct: " << xs);
  return Dim({1}, max(xs[0].bd, xs[1].bd));
}

string ConstScalarMultiply::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " * " << alpha;
  return s.str();
}

Dim ConstScalarMultiply::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "ConstScalarMultiply expects one argument: " << xs);
  return xs[0];
}

string DotProduct::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << "^T . " << arg_names[1];
  return s.str();
}

Dim DotProduct::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2 &&
                          xs[0].single_batch() == xs[1].single_batch(),
                          "Bad arguments to DotProduct: " << xs);
  return Dim({1}, max(xs[0].bd, xs[1].bd));
}

string Transpose::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "transpose("<< arg_names[0] << ", ";
  for(size_t i = 0; i < dims.size(); ++i)
    s << (i == 0?'{':',') << dims[i];
  s << "})";
  return s.str();
}

Dim Transpose::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Bad arguments to Transpose: " << xs);
  DYNET_ARG_CHECK(xs[0].nd == dims.size() || xs[0].num_nonone_dims() == 1, "Dimensions passed to transpose (" << dims.size() << ") must be equal to dimensions in input tensor (" << xs[0].nd << ')');
  Dim ret(xs[0]);
  ret.nd = dims.size();
  for(size_t i = 0; i < dims.size(); ++i)
    ret.d[i] = xs[0][dims[i]];
  return ret;
}

string Reshape::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "reshape(" << arg_names[0] << " --> " << to << ')';
  return s.str();
}

Dim Reshape::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Reshape")
  if(to.size() == xs[0].size()) {
    return to;
  } else {
    DYNET_ARG_CHECK(to.batch_elems() == 1 && to.batch_size() == xs[0].batch_size(),
                    "Bad arguments to Reshape: " << to << ", " << xs[0]);
    Dim ret(to);
    ret.bd = xs[0].batch_elems();
    return ret;
  }
}

string KMHNGram::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "kmh-ngram(" << arg_names[0] << ')';
  return s.str();
}

Dim KMHNGram::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs[0].ndims() == 2, "Bad input dimensions in KMHNGram: " << xs);
  const unsigned new_cols = xs[0].cols() - n + 1;
  DYNET_ARG_CHECK(new_cols >= 1, "Bad input dimensions in KMHNGram: " << xs);
  return Dim({xs[0][0], new_cols});
}

string GaussianNoise::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " + N(0," << stddev << ')';
  return s.str();
}

Dim GaussianNoise::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in GaussianNoise")
  return xs[0];
}

string Dropout::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "dropout(" << arg_names[0] << ",p=" << p << ')';
  return s.str();
}

Dim Dropout::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Dropout")
  return xs[0];
}

string DropoutBatch::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "dropout_batch(" << arg_names[0] << ",p=" << p << ')';
  return s.str();
}

Dim DropoutBatch::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in DropoutBatch")
  return xs[0];
}

string DropoutDim::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "dropout_dim(" << arg_names[0] << ",p=" << p << ')';
  return s.str();
}

Dim DropoutDim::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in DropoutDim")
  DYNET_ARG_CHECK(xs[0].nd < 4, "DropoutDim only supports tensor up to order 3 + batch dimension, got tensor of order"<<xs[0].nd)
  DYNET_ARG_CHECK(xs[0].nd > dimension, "In DropoutDim : tried to drop along dimension "<<dimension<<" on tensor of order"<<xs[0].nd)
  return xs[0];
}

string BlockDropout::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "block_dropout(" << arg_names[0] << ",dropout_probability=" << dropout_probability << ')';
  return s.str();
}

Dim BlockDropout::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in BlockDropout")
  return xs[0];
}

string ConstantPlusX::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << c << " + " << arg_names[0];
  return s.str();
}

Dim ConstantPlusX::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in ConstantPlusX")
  return xs[0];
}

string ConstantMinusX::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << c << " - " << arg_names[0];
  return s.str();
}

Dim ConstantMinusX::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in ConstantMinusX")
  return xs[0];
}

string LogSumExp::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "log(exp " << arg_names[0];
  for (unsigned i = 1; i < arg_names.size(); ++i)
    s << " + exp " << arg_names[i];
  s << ")";
  return s.str();
}

Dim LogSumExp::dim_forward(const vector<Dim>& xs) const {
  Dim d = xs[0].truncate();
  for (unsigned i = 1; i < xs.size(); ++i) {
    DYNET_ARG_CHECK(d.single_batch() == xs[i].truncate().single_batch(),
                            "Mismatched input dimensions in LogSumExp: " << xs);
    d.bd = max(xs[i].bd, d.bd);
  }
  return d;
}
string Sum::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0];
  for (unsigned i = 1; i < arg_names.size(); ++i)
    s << " + " << arg_names[i];
  return s.str();
}

int Sum::autobatch_sig(const ComputationGraph &cg, SigMap &sm) const {
  Sig s(nt::sum);
  s.add_node(args.size());
  // Two cases:
  // If unbatched, it's just an elementwise addition
  // TODO: This will be more efficient if we identify arguments that are used
  //       multiple times (e.g. bias vectors)
  if(dim.bd == 1) {
    s.add_int(-2);
  // Otherwise, make sure the dimensions match and that batched nodes don't intersect
  } else {
    s.add_dim(dim);
    for(auto ai : args) {
      s.add_int(cg.nodes[ai]->dim.bd == 1 ? ai : -1);
    }
  }
  return sm.get_idx(s);
}

std::vector<int> Sum::autobatch_concat(const ComputationGraph & cg) const {
  vector<int> ret(args.size(), 1);
  // If batched, true if multiple batched input as well
  if(dim.bd != 1)
    for(size_t i = 0; i < args.size(); ++i)
      ret[i] = cg.nodes[args[i]]->dim.bd == 1 ? 0 : 1;
  return ret;
}


Dim Sum::dim_forward(const vector<Dim>& xs) const {
  Dim d = xs[0].truncate();
  unsigned int batch = d.bd;
  for (unsigned i = 1; i < xs.size(); ++i) {
    DYNET_ARG_CHECK(d.single_batch() == xs[i].truncate().single_batch(),
                            "Mismatched input dimensions in Sum: " << xs);
    batch = max(xs[i].bd, batch);
  }
  d = xs[0]; d.bd = batch;
  return d;
}

string CwiseSum::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0];
  for (unsigned i = 1; i < arg_names.size(); ++i)
    s << " + " << arg_names[i];
  return s.str();
}

Dim CwiseSum::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in CwiseSum")
  Dim d = xs[1];
  DYNET_ARG_CHECK(xs[0].nd == xs[1].nd || xs[0].batch_size()==1 || xs[1].batch_size()==1, "CwiseSum: arguments must have equal number of dimensions, or have a scalar as one of its arguments.");
  for(int i=0; i<xs[0].nd; i++)
    DYNET_ARG_CHECK(xs[0].d[i]==xs[1].d[i] || xs[0].d[i]==1, "CwiseSum: For each dimension, the dim size needs to match or equal 1.");
  DYNET_ARG_CHECK(xs[0].bd==xs[1].bd || xs[0].bd==1, "CwiseSum: batch size must match or equal 1");
  return d;
}

string SumElements::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "sum_elems( " << arg_names[0] << " )";
  return s.str();
}

Dim SumElements::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in SumElements")
  return Dim({1}, xs[0].bd);
}

string SumBatches::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "sum_batches( " << arg_names[0] << " )";
  return s.str();
}

Dim SumBatches::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in SumBatches")
  return xs[0].single_batch();
}

string MomentElements::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "moment_elems( expression=" << arg_names[0] << ", order=" << order << " )";
  return s.str();
}

Dim MomentElements::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in MomentElements")
  DYNET_ARG_CHECK(order>= 1, "Order of moment should be >=1 in MomentElements (recieved "<<order<<")")
  return Dim({1}, xs[0].bd);
}

string StdElements::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "std_elems( expression=" << arg_names[0] << " )";
  return s.str();
}

Dim StdElements::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in StdElements")
  return Dim({1}, xs[0].bd);
}

string StdDimension::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "std_dim(expression=" << arg_names[0] << ',';
  for(size_t i = 0; i < dims.size(); ++i)
    s << (i == 0?'{':',') << dims[i];
  s << "})";
  return s.str();
}

Dim StdDimension::dim_forward(const vector<Dim>& xs) const {
  DYNET_ASSERT(xs.size() == 1, "Failed input count check in StdDimension");
  DYNET_ARG_CHECK(xs[0].nd <= 3, "StdDimension implemented up to tensors of order 3 (with minibatch) for now")
  for(int i=0; i<dims.size(); i++)
    DYNET_ARG_CHECK(dims[i] <= xs[0].nd, "dimension " << dims[i]<< " is out of bounds of tensor of order " << xs[0].nd << " in StdDimension" )
  DYNET_ARG_CHECK(dims.size()<=2, "Number of dimensions to reduce (excluding batch dimension) implemented up to 2 in StdDimension (received "<< dims.size() <<")")
  if(dims.size()==0)
    DYNET_ARG_CHECK(include_batch_dim, "At least one dimension has to be reduced (including batch dimension) in StdDimension")
  Dim ret(xs[0]);
  ret.delete_dims(dims, include_batch_dim);
  return ret;
}

string MomentDimension::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "moment_dim(expression=" << arg_names[0] << ',';
  for(size_t i = 0; i < dims.size(); ++i)
    s << (i == 0?'{':',') << dims[i];
  s << "}), order="<<order;
  return s.str();
}

Dim MomentDimension::dim_forward(const vector<Dim>& xs) const {
  DYNET_ASSERT(xs.size() == 1, "Failed input count check in MomentDimension");
  DYNET_ARG_CHECK(xs[0].nd <= 3, "MomentDimension implemented up to tensors of order 3 (with minibatch) for now")
  for(int i=0; i<dims.size(); i++)
    DYNET_ARG_CHECK(dims[i] <= xs[0].nd, "dimension " << dims[i]<< " is out of bounds of tensor of order " << xs[0].nd << " in MomentDimension" )
  DYNET_ARG_CHECK(order>= 1, "Order of moment should be >=1 in MomentDimension (received "<<order<<")")
  DYNET_ARG_CHECK(dims.size()<=2, "Number of dimensions to reduce (excluding batch dimension) implemented up to 2 in MomentDimension (received "<< dims.size() <<")")
  if(dims.size()==0)
    DYNET_ARG_CHECK(include_batch_dim, "At least one dimension has to be reduced (including batch dimension) in MomentDimension")
  Dim ret(xs[0]);
  ret.delete_dims(dims, include_batch_dim);
  return ret;
}

string Average::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "average(" << arg_names[0];
  for (unsigned i = 1; i < arg_names.size(); ++i)
    s << ", " << arg_names[i];
  s << ")";
  return s.str();
}

Dim Average::dim_forward(const vector<Dim>& xs) const {
  Dim d(xs[0]);
  for (unsigned i = 1; i < xs.size(); ++i) {
    DYNET_ARG_CHECK(xs[0].single_batch() == xs[i].single_batch(),
                            "Mismatched input dimensions in Average: " << xs);
    d.bd = max(xs[i].bd, d.bd);
  }
  return d;
}

string Sqrt::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "sqrt(" << arg_names[0] << ')';
  return s.str();
}

Dim Sqrt::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Sqrt")
  return xs[0];
}

string Abs::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "abs(" << arg_names[0] << ')';
  return s.str();
}

Dim Abs::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Abs")
  return xs[0];
}


string Erf::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "erf(" << arg_names[0] << ')';
  return s.str();
}

Dim Erf::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Erf")
  return xs[0];
}

string Tanh::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "tanh(" << arg_names[0] << ')';
  return s.str();
}

Dim Tanh::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Tanh")
  return xs[0];
}

string Square::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "square(" << arg_names[0] << ')';
  return s.str();
}

Dim Square::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Square")
  return xs[0];
}

string Cube::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "cube(" << arg_names[0] << ')';
  return s.str();
}

Dim Cube::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Cube")
  return xs[0];
}

string Exp::as_string(const vector<string>& arg_names) const {
  ostringstream os;
  os << "exp(" << arg_names[0] << ')';
  return os.str();
}

Dim Exp::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Exp")
  return xs[0];
}

string LogGamma::as_string(const vector<string>& arg_names) const {
  ostringstream os;
  os << "lgamma(" << arg_names[0] << ')';
  return os.str();
}

Dim LogGamma::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in LogGamma")
  return xs[0];
}

string Log::as_string(const vector<string>& arg_names) const {
  ostringstream os;
  os << "log(" << arg_names[0] << ')';
  return os.str();
}

Dim Log::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Log")
  return xs[0];
}

string Concatenate::as_string(const vector<string>& arg_names) const {
  ostringstream os;
  os << "concat({" << arg_names[0];
  for (unsigned i = 1; i < arg_names.size(); ++i) {
    os << ',' << arg_names[i];
  }
  os << "}, " << dimension << ')';
  return os.str();
}

Dim Concatenate::dim_forward(const vector<Dim>& xs) const {
  unsigned new_rows = 0;
  Dim dr = xs[0];
  for (auto c : xs) {
    if(dr.nd < c.nd) dr.resize(c.nd);
    if(c.nd < dr.nd) c.resize(dr.nd);
    new_rows += c[dimension];
    dr.set(dimension, c[dimension]);
    DYNET_ARG_CHECK(dr.single_batch() == c.single_batch(),
                            "Bad input dimensions in Concatenate: " << xs);
    dr.bd = max(dr.bd, c.bd);
  }
  dr.nd = max(xs[0].nd, dimension+1);
  dr.set(dimension, new_rows);
  return dr;
}

int Concatenate::autobatch_sig(const ComputationGraph &cg, SigMap &sm) const {
  Sig s(nt::concat);
  for (auto arg:args) s.add_dim(cg.nodes[arg]->dim);
  return sm.get_idx(s);
}


string ConcatenateToBatch::as_string(const vector<string>& arg_names) const {
  ostringstream os;
  os << "concat_batch_elems(" << arg_names[0];
  for (unsigned i = 1; i < arg_names.size(); ++i) {
    os << ',' << arg_names[i];
  }
  os << ')';
  return os.str();
}

Dim ConcatenateToBatch::dim_forward(const vector<Dim>& xs) const {
  DYNET_ASSERT(xs.size() > 0, "Failed input count check in ConcatenateToBatch")
  Dim d(xs[0]);
  for (unsigned i = 1; i < xs.size(); ++i) {
    DYNET_ARG_CHECK(xs[0].single_batch() == xs[i].single_batch(),
                            "Mismatched input dimensions in ConcatenateToBatch: " << xs);
    d.bd += xs[i].bd;
  }
  return d;
}

string PairwiseRankLoss::as_string(const vector<string>& arg_names) const {
  ostringstream os;
  os << "max(0, " << margin << " - " << arg_names[0] << " + " << arg_names[1] << ')';
  return os.str();
}

Dim PairwiseRankLoss::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2 &&
                          xs[0] == xs[1] &&
                          xs[0].rows() == 1 &&
                          (xs[0].ndims() == 1 || xs[0].ndims() == 2),
                          "Bad input dimensions in PairwiseRankLoss: " << xs);
  return xs[0].bd >= xs[1].bd ? xs[0] : xs[1];
}

string Identity::as_string(const vector<string>& arg_names) const {
  return arg_names[0];
}

Dim Identity::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Identity")
  return xs[0];
}

string NoBackprop::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "nobackprop(" << arg_names[0] << ')';
  return s.str();
}

Dim NoBackprop::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in NoBackprop")
  return xs[0];
}

string FlipGradient::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "flip_gradient(" << arg_names[0] << ')';
  return s.str();
}

Dim FlipGradient::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in FlipGradient");
  return xs[0];
}  
  
string Softmax::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "softmax(" << arg_names[0] << ')';
  return s.str();
}

Dim Softmax::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Softmax");
  DYNET_ARG_CHECK(xs[0].nd <= 2, "Bad input dimensions in Softmax, must be 2 or fewer: " << xs);
  return xs[0];
}

int Softmax::autobatch_sig(const ComputationGraph & cg, SigMap &sm) const {
  Sig s(nt::softmax);
  s.add_dim(dim);
  return sm.get_idx(s);
}
std::vector<int> Softmax::autobatch_concat(const ComputationGraph & cg) const {
  return vector<int>(1, 1);
}

string SoftSign::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "softsign(" << arg_names[0] << ')';
  return s.str();
}

Dim SoftSign::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in SoftSign");
  DYNET_ARG_CHECK(LooksLikeVector(xs[0]), "Bad input dimensions in SoftSign: " << xs);
  return xs[0];
}

string LogSoftmax::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "log_softmax(" << arg_names[0] << ')';
  return s.str();
}

Dim LogSoftmax::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in LogSoftmax")
  DYNET_ARG_CHECK(xs[0].nd <= 2, "Bad input dimensions in LogSoftmax, must be 2 or fewer: " << xs);
  return xs[0];
}

string RestrictedLogSoftmax::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "r_log_softmax(" << arg_names[0] << ')';
  return s.str();
}

Dim RestrictedLogSoftmax::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in RestrictedLogSoftmax")
  DYNET_ARG_CHECK(LooksLikeVector(xs[0]), "Bad input dimensions in RestrictedLogSoftmax: " << xs);
  return xs[0];
}

string PickElement::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "pick(" << arg_names[0] << ',';
  if(pval) { 
    s << *pval;
  } else {
    DYNET_ASSERT(pvals, "Have neither index nor index vector in PickElement");
    s << '[';
    if(pvals->size()) {
      s << (*pvals)[0];
      for(size_t i = 1; i < pvals->size(); ++i)
        s << ',' << (*pvals)[i];
    }
    s << "]";
  }
  s << ", " << dimension << ")";
  return s.str();
}

Dim PickElement::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in PickElement");
  DYNET_ARG_CHECK(dimension < xs[0].nd,
                          "Tried to PickElement on dimension " << dimension << " bigger than input " << xs[0]);
  DYNET_ARG_CHECK(xs[0].nd < 4,
                          "PickElement not currently supported for tensors of 4 or more dimensions.");
  
  Dim ret(xs[0]);
  if (pvals){
    DYNET_ARG_CHECK(xs[0].bd == 1 || xs[0].bd == pvals->size(),
                          "Number of elements in the passed-in index vector (" <<  pvals->size() << ")"
                            " did not match number of elements in mini-batch elements in expression (of dimension " << xs[0].bd << ") in PickElement");
    ret.bd = pvals->size();
  }

  ret.delete_dim(dimension);
  return ret;
}

// x_1 is a vector
// y = (x_1)[start:end]
string PickRange::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "slice(" << arg_names[0] << ',' << start << ':' << end << ", dim=" << dim << ')';
  return s.str();
}

Dim PickRange::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in PickRange");
  DYNET_ARG_CHECK(dim < xs[0].nd && start < end && xs[0][dim] >= end,
                          "Bad input dimensions or range in PickRange: " << xs << " range(" << start << ", " << end << ") with dim=" << dim);
  Dim ret = xs[0]; ret.d[dim] = end-start;
  return ret;
}

int PickRange::autobatch_sig(const ComputationGraph & cg, SigMap &sm) const {
  Sig s(nt::pickrange);
  const Dim &in_dim = cg.nodes[args[0]]->dim;
  s.add_dim(in_dim);
  s.add_node(start);
  s.add_node(end);
  return sm.get_idx(s);
}

string PickBatchElements::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "pick_batch_elems(" << arg_names[0] << ',';
  if (pval) {
    s << *pval;
  } else {
    DYNET_ASSERT(pvals, "Have neither index nor index vector in PickBatchElements");
    s << '[';
    if (pvals->size()) {
      s << (*pvals)[0];
      for (size_t i = 1; i < pvals->size(); ++i)
        s << ',' << (*pvals)[i];
    }
    s << "]";
  }
  s << ")";
  return s.str();
}

Dim PickBatchElements::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in PickBatchElements")
  DYNET_ARG_CHECK(xs[0].nd < 4, "PickElement not currently supported for tensors of 4 or more dimensions.");
  Dim ret(xs[0]);
  if (pval) {
    // set batch size to one.
    ret.bd = 1;
  } else {
    DYNET_ASSERT(pvals, "Have neither index nor index vector in PickBatchElements");
    ret.bd = pvals->size();
  }
  return ret;
}

string CwiseMultiply::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " \\cdot " << arg_names[1];
  return s.str();
}

Dim CwiseMultiply::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in CwiseMultiply")
  Dim d = xs[1];

  DYNET_ARG_CHECK(xs[0].nd == xs[1].nd || xs[0].batch_size()==1 || xs[1].batch_size()==1, "CwiseMultiply: arguments must have equal number of dimensions, or have a scalar as one of its arguments.");
  for(int i=0; i<xs[0].nd; i++)
    DYNET_ARG_CHECK(xs[0].d[i]==xs[1].d[i] || xs[0].d[i]==1, "CwiseMultiply: For each dimension, the dim size needs to match or equal 1.");
  DYNET_ARG_CHECK(xs[0].bd==xs[1].bd || xs[0].bd==1, "CwiseMultiply: batch size must match or equal 1");
  return d;
}

int CwiseMultiply::autobatch_sig(const ComputationGraph & cg, SigMap &sm) const {
  // TODO: This does not handle the case where dimensions differ
  Sig s(nt::cmult);
  return cg.nodes[args[0]]->dim == cg.nodes[args[1]]->dim ? sm.get_idx(s) : 0;
}

std::vector<int> CwiseMultiply::autobatch_concat(const ComputationGraph & cg) const {
  return vector<int>(2, 1);
}

string ScalarAdd::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " + " << arg_names[1];
  return s.str();
}

Dim ScalarAdd::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in ScalarAdd")
  Dim d = xs[0].truncate();
  DYNET_ARG_CHECK(xs[1].batch_size() == 1,
                          "Mismatched input dimensions in ScalarAdd: " << xs);
  d.bd = max(xs[1].bd, d.bd);
  return d;
}

string ScalarMultiply::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " \\cdot " << arg_names[1];
  return s.str();
}

Dim ScalarMultiply::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in ScalarMultiply")
  Dim d = xs[1];
  DYNET_ARG_CHECK(xs[0].batch_size() == 1,
                          "Mismatched input dimensions in ScalarMultiply: " << xs);
  d.bd = max(xs[0].bd, d.bd);
  return d;
}

string ScalarQuotient::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " / " << arg_names[1];
  return s.str();
}

Dim ScalarQuotient::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in ScalarQuotient")
  Dim d = xs[0].truncate();
  DYNET_ARG_CHECK(xs[1].batch_size() == 1,
                          "Mismatched input dimensions in ScalarQuotient: " << xs);
  d.bd = max(xs[1].bd, d.bd);
  return d;
}


string Pow::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " ** " << arg_names[1];
  return s.str();
}

Dim Pow::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in Pow")
  Dim d = xs[0].truncate();
  DYNET_ARG_CHECK(xs[1].truncate().single_batch().size() == 1, "Bad input dimensions in Pow: " << xs);
  return d;
}

string CwiseQuotient::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " / " << arg_names[1];
  return s.str();
}

Dim CwiseQuotient::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in CwiseQuotient")
  Dim d = (xs[0].size()>=xs[1].size()) ? xs[0] : xs[1];
  DYNET_ARG_CHECK(xs[0].nd == xs[1].nd || xs[0].batch_size()==1 || xs[1].batch_size()==1, "CwiseQuotient: arguments must have equal number of dimensions, or have a scalar as one of its arguments.");
  for(int i=0; i<xs[0].nd; i++)
    DYNET_ARG_CHECK(xs[0].d[i]==xs[1].d[i] || (xs[0].d[i]==1 && xs[0].size() < xs[1].size()) || (xs[1].d[i]==1 && xs[0].size() > xs[1].size()),
		    "CwiseQuotient: For each dimension, the dim size needs to match or equal 1.");
  DYNET_ARG_CHECK(xs[0].bd==xs[1].bd || (xs[0].bd==1 && xs[0].size() < xs[1].size()) || (xs[1].bd==1 && xs[0].size() > xs[1].size()),
		  "CwiseQuotient: batch size must match or equal 1");
  return d;
}

string Negate::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << '-' << arg_names[0];
  return s.str();
}

Dim Negate::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Negate");
  return xs[0];
}

string Rectify::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "ReLU(" << arg_names[0] << ')';
  return s.str();
}

Dim Rectify::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in Rectify");
  return xs[0];
}

string ExponentialLinearUnit::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "ELU(" << arg_names[0] << ", lambda=" << lambda << ", alpha=" << alpha << ')';
  return s.str();
}

Dim ExponentialLinearUnit::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in ExponentialLinearUnit");
  return xs[0];
}

string HuberDistance::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "|| " << arg_names[0] << " - " << arg_names[1] << " ||_H(" << d << ')';
  return s.str();
}

Dim HuberDistance::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in HuberDistance");
  DYNET_ARG_CHECK(xs[0].single_batch() == xs[1].single_batch() ||
                          (LooksLikeVector(xs[0]) && LooksLikeVector(xs[1]) && xs[0].batch_size() == xs[1].batch_size()),
                          "Mismatched input dimensions in HuberDistance: " << xs);
  return Dim({1}, max(xs[0].bd, xs[1].bd));
}

string L1Distance::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "|| " << arg_names[0] << " - " << arg_names[1] << " ||_1";
  return s.str();
}

Dim L1Distance::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in L1Distance")
  DYNET_ARG_CHECK(xs[0].single_batch() == xs[1].single_batch() ||
                          (LooksLikeVector(xs[0]) && LooksLikeVector(xs[1]) && xs[0].batch_size() == xs[1].batch_size()),
                          "Mismatched input dimensions in L1Distance: " << xs);
  return Dim({1}, max(xs[0].bd, xs[1].bd));
}

string PoissonRegressionLoss::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "-log Poisson(" << pty << "; lambda=\\exp" << arg_names[0] << ')';
  return s.str();
}

Dim PoissonRegressionLoss::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1 && xs[0].size() == 1, "Bad input dimensions in PoissonRegressionLoss: " << xs);
  return xs[0];
}

string SquaredNorm::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "|| " << arg_names[0] << " ||^2";
  return s.str();
}

Dim SquaredNorm::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in SquaredNorm")
  return Dim({1}, xs[0].bd);
}

string L2Norm::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "|| " << arg_names[0] << " ||";
  return s.str();
}

Dim L2Norm::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in L2Norm")
  return Dim({1}, xs[0].bd);
}


string SquaredEuclideanDistance::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "|| " << arg_names[0] << " - " << arg_names[1] << " ||^2";
  return s.str();
}

Dim SquaredEuclideanDistance::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in SquaredEuclideanDistance")
  DYNET_ARG_CHECK(xs[0].single_batch() == xs[1].single_batch() ||
                          (LooksLikeVector(xs[0]) && LooksLikeVector(xs[1]) && xs[0].batch_size() == xs[1].batch_size()),
                          "Bad input dimensions in SquaredEuclideanDistance: " << xs);
  return Dim({1}, max(xs[0].bd, xs[1].bd));
}

int SquaredEuclideanDistance::autobatch_sig(const ComputationGraph & cg, SigMap &sm) const {
  Sig s(nt::squared_distance);
  const Dim &dleft = cg.nodes[args[0]]->dim, &dright = cg.nodes[args[1]]->dim;
  if(dleft.bd == dright.bd) {
    s.add_node(1);
    s.add_dim(dleft);
  } else if(dleft.bd == 1) {
    s.add_node(2);
    s.add_node(args[0]);
    s.add_dim(dright);
  } else {
    s.add_node(3);
    s.add_node(args[1]);
    s.add_dim(dleft);
  }
  return sm.get_idx(s);
}
std::vector<int> SquaredEuclideanDistance::autobatch_concat(const ComputationGraph & cg) const {
  const Dim &dleft = cg.nodes[args[0]]->dim, &dright = cg.nodes[args[1]]->dim;
  vector<int> ret(2, 1);
  if(dleft.bd != dright.bd) {
    if(dleft.bd == 1)
      ret[0] = 0;
    else
      ret[1] = 0;
  }
  return ret;
}

string LogisticSigmoid::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "\\sigma(" << arg_names[0] << ')';
  return s.str();
}

Dim LogisticSigmoid::dim_forward(const vector<Dim>& xs) const {
  DYNET_ASSERT(xs.size() == 1, "Failed input count check in LogisticSigmoid")
  return xs[0];
}

string BinaryLogLoss::as_string(const vector<string>& arg_names) const {
  ostringstream os;
  os << "binary_log_loss(" << arg_names[0] << ", " << arg_names[1] << ')';
  return os.str();
}

Dim BinaryLogLoss::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in BinaryLogLoss")
  DYNET_ARG_CHECK(xs[0].rows() == 2 || xs[0].ndims() == 1, "Bad input dimensions in BinaryLogLoss: " << xs);
  DYNET_ARG_CHECK(xs[1].rows() == 2 || xs[1].ndims() == 1, "Bad input dimensions in BinaryLogLoss: " << xs);
  return Dim({1}, max(xs[0].bd, xs[1].bd));
}

string Zeroes::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "zeroes(" << dim << ')';
  return s.str();
}

Dim Zeroes::dim_forward(const vector<Dim>& xs) const {
  return dim;
}

string RandomNormal::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "random_normal(" << dim << ')';
  return s.str();
}

Dim RandomNormal::dim_forward(const vector<Dim>& xs) const {
  return dim;
}

string RandomBernoulli::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "random_bernoulli(" << dim << ", " << p << ')';
  return s.str();
}

Dim RandomBernoulli::dim_forward(const vector<Dim>& xs) const {
  return dim;
}

string RandomUniform::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "random_uniform(" << dim << ", " << left << ", " << right << ')';
  return s.str();
}

Dim RandomUniform::dim_forward(const vector<Dim>& xs) const {
  return dim;
}

string RandomGumbel::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "random_gumbel(" << dim << ", " << mu << ", " << beta << ')';
  return s.str();
}

Dim RandomGumbel::dim_forward(const vector<Dim>& xs) const {
  return dim;
}

string MaxDimension::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "max_dim(" << arg_names[0] << ", reduced_dim=" << reduced_dim << ')';
  return s.str();
}

Dim MaxDimension::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in MaxDimension");
  DYNET_ARG_CHECK(reduced_dim < xs[0].nd,
                          "Tried to MaxDimension on dimension " << reduced_dim << " bigger than input " << xs[0]);
  DYNET_ARG_CHECK(xs[0].nd < 4,
                          "MaxDimension not currently supported for tensors of 4 or more dimensions.");
  Dim ret(xs[0]);
  ret.delete_dim(reduced_dim);
  return ret;
}

string MinDimension::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "min_dim(" << arg_names[0] << ", reduced_dim=" << reduced_dim << ')';
  return s.str();
}

Dim MinDimension::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in MinDimension");
  DYNET_ARG_CHECK(reduced_dim < xs[0].nd,
                          "Tried to MinDimension on dimension " << reduced_dim << " bigger than input " << xs[0]);
  DYNET_ARG_CHECK(xs[0].nd < 4,
                          "MinDimension not currently supported for tensors of 4 or more dimensions.");
  Dim ret(xs[0]);
  ret.delete_dim(reduced_dim);
  return ret;
}

string WeightNormalization::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "weight_norm(" << arg_names[0] << ", " << arg_names[1] << ')';
  return s.str();
}

Dim WeightNormalization::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in WeightNormalization");
  DYNET_ARG_CHECK(1 == xs[1].size()," Size of gain parameter in WeightNormalization should be 1, received " << xs[1].size());
  return xs[0];
}

} // namespace dynet
