import numpy as np
import matplotlib.pyplot as plt

plt.figure(figsize=(15, 12))

# =========================================================
# 1. Dynamics: Order Parameter over Time
# =========================================================
ax1 = plt.subplot(2, 2, 1)
data_dyn = np.loadtxt("dynamics.dat")
t = data_dyn[:, 0]
r_sub = data_dyn[:, 1]
r_sup = data_dyn[:, 2]

ax1.plot(
    t,
    r_sup,
    color="purple",
    linewidth=2,
    label=r"$K = 2.5 > K_c$ (Grows to $r_\infty$)",
)
ax1.plot(t, r_sub, color="gray", linewidth=2, label=r"$K = 0.5 < K_c$ (Decays)")
ax1.set_xlabel("Time (t)", fontsize=12)
ax1.set_ylabel("Order Parameter (r)", fontsize=12)
ax1.set_title("Evolution of $r(t)$ (Normal Dist)", fontsize=14)
ax1.legend()
ax1.grid(True)

# =========================================================
# 2. Transition Curve + Cauchy Analytics
# =========================================================
ax2 = plt.subplot(2, 2, 2)
data_uni = np.loadtxt("transition_uniform.dat")
data_norm = np.loadtxt("transition_normal.dat")
data_cauchy = np.loadtxt("transition_cauchy.dat")

ax2.plot(
    data_uni[:, 0],
    data_uni[:, 1],
    "o-",
    markersize=4,
    label=r"Uniform $\omega \in[-1, 1]$",
)
ax2.plot(
    data_norm[:, 0],
    data_norm[:, 1],
    "s-",
    markersize=4,
    label=r"Normal $\mu=0, \sigma=0.5$",
)
ax2.plot(
    data_cauchy[:, 0],
    data_cauchy[:, 1],
    "d-",
    markersize=4,
    label=r"Cauchy $\gamma=0.5$",
)

gamma = 0.5
Kc_cauchy = 2 * gamma
K_vals = np.linspace(Kc_cauchy, 4.0, 100)
r_analytical = np.sqrt(1 - Kc_cauchy / K_vals)
ax2.plot(
    K_vals,
    r_analytical,
    "k--",
    linewidth=2,
    label=r"Analytical Cauchy: $r = \sqrt{1 - K_c/K}$",
)

ax2.set_xlabel("Coupling Strength (K)", fontsize=12)
ax2.set_ylabel(r"Steady-State Order Parameter ($r_\infty$)", fontsize=12)
ax2.set_title("Transition to Synchronization", fontsize=14)
ax2.legend()
ax2.grid(True)

# =========================================================
# 3. Locked vs Drifting Oscillators
# =========================================================
ax3 = plt.subplot(2, 2, 3)
snap_sup = np.loadtxt("snapshot_supercritical.dat")
omega = snap_sup[:, 0]
eff_freq = snap_sup[:, 1]

# Sort by omega for a clean line plot
sort_idx = np.argsort(omega)
omega_sorted = omega[sort_idx]
eff_freq_sorted = eff_freq[sort_idx]

ax3.plot(
    omega_sorted,
    omega_sorted,
    "k--",
    alpha=0.5,
    label=r"Uncoupled ($\dot{\theta}_i = \omega_i$)",
)
ax3.scatter(
    omega, eff_freq, c="red", s=10, alpha=0.6, label=r"Coupled $\dot{\theta}_i$ (K=1.0)"
)

# Highlight the locked plateau
ax3.axhline(0, color="blue", linestyle=":", alpha=0.7)
ax3.set_xlim(-2.5, 2.5)
ax3.set_ylim(-2.5, 2.5)
ax3.set_xlabel(r"Natural Frequency ($\omega_i$)", fontsize=12)
ax3.set_ylabel(r"Effective Frequency ($\dot{\theta}_i$)", fontsize=12)
ax3.set_title("Locked vs. Drifting Oscillators", fontsize=14)
ax3.legend()
ax3.grid(True)

# =========================================================
# 4. Polar Histogram/Scatter of Phases
# =========================================================
ax4 = plt.subplot(2, 2, 4, projection="polar")
snap_sub = np.loadtxt("snapshot_subcritical.dat")
snap_sup = np.loadtxt("snapshot_supercritical.dat")
phases_sub = snap_sub[:, 2]
phases_sup = snap_sup[:, 2]

# Plot Subcritical (Scattered)
ax4.scatter(
    phases_sub,
    np.ones_like(phases_sub) * 0.8,
    alpha=0.3,
    s=20,
    color="gray",
    label=r"Scattered ($K<K_c$)",
)
# Plot Supercritical (Clustered)
ax4.scatter(
    phases_sup,
    np.ones_like(phases_sup) * 1.0,
    alpha=0.5,
    s=20,
    color="purple",
    label=r"Clustered ($K>K_c$)",
)
ax4.set_ylim(0, 1.2)
ax4.set_yticks([])
ax4.set_title("Phase Distribution", fontsize=14)
ax4.legend(loc="upper right")

plt.tight_layout()
plt.show()
