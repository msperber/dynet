#include "dynet/rand.h"
#include "dynet/nodes-random.h"

#include "dynet/nodes-macros.h"

using namespace std;

namespace dynet {

// ************* GaussianNoise *************

#ifndef __CUDACC__

string GaussianNoise::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " + N(0," << stddev << ')';
  return s.str();
}

Dim GaussianNoise::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 1, "Failed input count check in GaussianNoise")
  return xs[0];
}

#endif

template<class MyDevice>
void GaussianNoise::forward_dev_impl(const MyDevice & dev, const vector<const Tensor*>& xs, Tensor& fx) const {
  Eigen::internal::NormalRandomGenerator<float> normal_rg(draw_random_seed());
  fx.tvec().device(*dev.edevice) = xs[0]->tvec() + xs[0]->tvec().random(normal_rg) * stddev;
}

template<class MyDevice>
void GaussianNoise::backward_dev_impl(const MyDevice & dev,
                             const vector<const Tensor*>& xs,
                             const Tensor& fx,
                             const Tensor& dEdf,
                             unsigned i,
                             Tensor& dEdxi) const {
  dEdxi.tvec().device(*dev.edevice) += dEdf.tvec();
}
DYNET_NODE_INST_DEV_IMPL(GaussianNoise)

// ************* RandomNormal *************

#ifndef __CUDACC__

string RandomNormal::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "random_normal(" << dim << ')';
  return s.str();
}

Dim RandomNormal::dim_forward(const vector<Dim>& xs) const {
  return dim;
}

#endif

template<class MyDevice>
void RandomNormal::forward_dev_impl(const MyDevice & dev, const vector<const Tensor*>& xs, Tensor& fx) const {
  DYNET_ASSERT(xs.size() == 0, "Failed dimension check in RandomNormal::forward");
  Eigen::internal::NormalRandomGenerator<float> normal_rg(draw_random_seed());
  fx.tvec().device(*dev.edevice) = fx.tvec().random(normal_rg);
}

template<class MyDevice>
void RandomNormal::backward_dev_impl(const MyDevice & dev,
                             const vector<const Tensor*>& xs,
                             const Tensor& fx,
                             const Tensor& dEdf,
                             unsigned i,
                             Tensor& dEdxi) const {
  DYNET_RUNTIME_ERR("Called backward() on an arity 0 node");
}
DYNET_NODE_INST_DEV_IMPL(RandomNormal)

// ************* RandomBernoulli *************

#ifndef __CUDACC__

string RandomBernoulli::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "random_bernoulli(" << dim << ", " << p << ')';
  return s.str();
}

Dim RandomBernoulli::dim_forward(const vector<Dim>& xs) const {
  return dim;
}

#endif


#ifdef HAVE_CUDA
__global__
void dy_sign(int n, float *x)
{
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  for (int i = index; i < n; i += stride){
    if (x[i]>0.0) x[i] = 1.0;
    else x[i] = 0.0;
  }
}
#endif

template<class MyDevice>
void RandomBernoulli::forward_dev_impl(const MyDevice & dev, const vector<const Tensor*>& xs, Tensor& fx) const {
  DYNET_ASSERT(xs.size() == 0, "Failed dimension check in RandomBernoulli::forward");
  Eigen::internal::UniformRandomGenerator<float> uni_rg(draw_random_seed());
  fx.tvec().device(*dev.edevice) = fx.tvec().random(uni_rg);
//  fx.tvec().device(*dev.edevice) = (fx.tvec() < fx.tvec().constant(p)).cast<float>() * scale;
//  fx.tvec().device(*dev.edevice) = (fx.tvec() + fx.tvec().constant(p - 0.5)).round() * scale;
#ifdef HAVE_CUDA
  fx.tvec().device(*dev.edevice) = fx.tvec() + fx.tvec().constant(p - 1.0);
  dy_sign<<<1, 256>>>(fx.d.size(), fx.v);
#else
  fx.tvec().device(*dev.edevice) = (fx.tvec() + fx.tvec().constant(p - 0.5)).round();
#endif
  fx.tvec().device(*dev.edevice) = fx.tvec() * scale;
}

template<class MyDevice>
void RandomBernoulli::backward_dev_impl(const MyDevice & dev,
                             const vector<const Tensor*>& xs,
                             const Tensor& fx,
                             const Tensor& dEdf,
                             unsigned i,
                             Tensor& dEdxi) const {
  DYNET_RUNTIME_ERR("Called backward() on an arity 0 node");
}
DYNET_NODE_INST_DEV_IMPL(RandomBernoulli)

// ************* RandomUniform *************

#ifndef __CUDACC__

string RandomUniform::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "random_uniform(" << dim << ", " << left << ", " << right << ')';
  return s.str();
}

Dim RandomUniform::dim_forward(const vector<Dim>& xs) const {
  return dim;
}

#endif

template<class MyDevice>
void RandomUniform::forward_dev_impl(const MyDevice & dev, const vector<const Tensor*>& xs, Tensor& fx) const {
  DYNET_ASSERT(xs.size() == 0, "Failed dimension check in RandomUniform::forward");
  Eigen::internal::UniformRandomGenerator<float> uni_rg(draw_random_seed());
  fx.tvec().device(*dev.edevice) = (fx.tvec().random(uni_rg) * (right-left)) + left;
}

template<class MyDevice>
void RandomUniform::backward_dev_impl(const MyDevice & dev,
                             const vector<const Tensor*>& xs,
                             const Tensor& fx,
                             const Tensor& dEdf,
                             unsigned i,
                             Tensor& dEdxi) const {
  DYNET_RUNTIME_ERR("Called backward() on an arity 0 node");
}
DYNET_NODE_INST_DEV_IMPL(RandomUniform)

// ************* RandomGumbel *************

#ifndef __CUDACC__

string RandomGumbel::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << "random_gumbel(" << dim << ", " << mu << ", " << beta << ')';
  return s.str();
}

Dim RandomGumbel::dim_forward(const vector<Dim>& xs) const {
  return dim;
}

#endif

template<class MyDevice>
void RandomGumbel::forward_dev_impl(const MyDevice & dev, const vector<const Tensor*>& xs, Tensor& fx) const {
  DYNET_ASSERT(xs.size() == 0, "Failed dimension check in RandomGumbel::forward");
  DYNET_ARG_CHECK(mu == 0.0 && beta == 1.0, "RandomGumbel only supports Gumbel(0,1) at the moment (pull requests welcome)");
  Eigen::internal::UniformRandomGenerator<float> uni_rg(draw_random_seed());
  fx.tvec().device(*dev.edevice) = fx.tvec().random(uni_rg);
  float eps = 1e-20;
  fx.tvec().device(*dev.edevice) = -(-fx.tvec().cwiseMax(eps).log()).cwiseMax(eps).log();
}

template<class MyDevice>
void RandomGumbel::backward_dev_impl(const MyDevice & dev,
                             const vector<const Tensor*>& xs,
                             const Tensor& fx,
                             const Tensor& dEdf,
                             unsigned i,
                             Tensor& dEdxi) const {
  DYNET_RUNTIME_ERR("Called backward() on an arity 0 node");
}
DYNET_NODE_INST_DEV_IMPL(RandomGumbel)


}
