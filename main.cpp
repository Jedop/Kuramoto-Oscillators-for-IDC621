#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

constexpr int N = 1000;
constexpr double dt = 0.05;
constexpr int steps = 15000;
constexpr int transient = 5000;

// Mean-field update step
void euler_step(std::vector<double> &theta, const std::vector<double> &omega,
                double K, double &r, double &psi) {
  double sum_sin = 0.0, sum_cos = 0.0;

  for (int i = 0; i < N; i++) {
    sum_cos += std::cos(theta[i]);
    sum_sin += std::sin(theta[i]);
  }
  r = std::sqrt(sum_cos * sum_cos + sum_sin * sum_sin) / N;
  psi = std::atan2(sum_sin, sum_cos);

  std::vector<double> new_theta(N);
  for (int i = 0; i < N; i++) {
    new_theta[i] =
        theta[i] + dt * (omega[i] + K * r * std::sin(psi - theta[i]));
  }
  theta = new_theta;
}

// 1. Simulates dynamics (r vs t) for multiple Ks
void simulate_dynamics(const std::vector<double> &omega, double K1, double K2,
                       const std::string &filename) {
  std::ofstream file(filename);
  std::mt19937 gen(123);
  std::uniform_real_distribution<> phase_dist(0.0, 2.0 * M_PI);

  std::vector<double> theta1(N), theta2(N);
  for (int i = 0; i < N; i++) {
    theta1[i] = phase_dist(gen);
    theta2[i] = theta1[i]; // Start from same initial conditions
  }

  double r1, psi1, r2, psi2;
  for (int t = 0; t < steps; t++) {
    euler_step(theta1, omega, K1, r1, psi1);
    euler_step(theta2, omega, K2, r2, psi2);
    if (t % 10 == 0)
      file << t * dt << " " << r1 << " " << r2 << "\n";
  }
}

// 2. Sweeps K for transition curves
void simulate_transition(const std::vector<double> &omega,
                         const std::string &filename) {
  std::ofstream file(filename);
  std::mt19937 gen(123);
  std::uniform_real_distribution<> phase_dist(0.0, 2.0 * M_PI);

  for (double K = 0.0; K <= 4.0; K += 0.1) {
    std::vector<double> theta(N);
    for (int i = 0; i < N; i++)
      theta[i] = phase_dist(gen);

    double r, psi, avg_r = 0.0;
    for (int t = 0; t < steps; t++) {
      euler_step(theta, omega, K, r, psi);
      if (t > transient)
        avg_r += r;
    }
    file << K << " " << avg_r / (steps - transient) << "\n";
  }
}

// 3. Extracts final phases and effective frequencies for a specific K
// (Partially Synchronized State)
void simulate_snapshot(const std::vector<double> &omega, double K,
                       const std::string &filename) {
  std::ofstream file(filename);
  std::mt19937 gen(123);
  std::uniform_real_distribution<> phase_dist(0.0, 2.0 * M_PI);

  std::vector<double> theta(N);
  for (int i = 0; i < N; i++)
    theta[i] = phase_dist(gen);

  double r, psi;
  std::vector<double> theta_transient(N);

  for (int t = 0; t < steps; t++) {
    euler_step(theta, omega, K, r, psi);
    if (t == transient)
      theta_transient = theta; // Save state to calculate effective frequency
  }

  // Effective frequency is delta_theta / delta_time
  double time_elapsed = (steps - transient) * dt;
  for (int i = 0; i < N; i++) {
    double eff_freq = (theta[i] - theta_transient[i]) / time_elapsed;
    // Output: natural frequency, effective frequency, final phase
    file << omega[i] << " " << eff_freq << " " << fmod(theta[i], 2.0 * M_PI)
         << "\n";
  }
}

int main() {
  std::mt19937 gen(42);

  std::vector<double> omega_uniform(N);
  std::vector<double> omega_normal(N);
  std::vector<double> omega_cauchy(N);

  std::uniform_real_distribution<> uni_dist(-1.0, 1.0);
  std::normal_distribution<> norm_dist(0.0, 0.5);
  std::cauchy_distribution<> cauchy_dist(0.0, 0.5); // gamma (HWHM) = 0.5

  // Bound Cauchy to avoid extreme outliers messing up numerical integration
  for (int i = 0; i < N; i++) {
    omega_uniform[i] = uni_dist(gen);
    omega_normal[i] = norm_dist(gen);

    double c_val = cauchy_dist(gen);
    while (std::abs(c_val) > 20.0)
      c_val = cauchy_dist(gen);
    omega_cauchy[i] = c_val;
  }

  std::cout << "1. Dynamics (Normal dist)...\n";
  simulate_dynamics(omega_normal, 0.5, 2.5,
                    "dynamics.dat"); // K_c is approx 1.6 for Normal(0, 0.5)

  std::cout << "2. Transitions...\n";
  simulate_transition(omega_uniform, "transition_uniform.dat");
  simulate_transition(omega_normal, "transition_normal.dat");
  simulate_transition(omega_cauchy, "transition_cauchy.dat");

  std::cout << "3. Snapshots (Locked vs Drifting & Polar)...\n";
  simulate_snapshot(omega_normal, 0.5, "snapshot_subcritical.dat");
  simulate_snapshot(omega_normal, 1.0, "snapshot_supercritical.dat");

  std::cout << "Done!\n";
  return 0;
}
