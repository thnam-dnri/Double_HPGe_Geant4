# Geant4 Simulation of the HPGe Coincidence Detector System at Dalat NRI – Paper Outline

## 1. Introduction
High-purity germanium (HPGe) γ–γ coincidence spectrometers at research reactors are indispensable for high-resolution nuclear spectroscopy. By enforcing time-correlated detection of cascade γ-rays, the technique clarifies level schemes, suppresses background with high selectivity, and exposes weak branches that are easily hidden in singles spectra. These advantages make coincidence measurements central to constructing reliable decay schemes and identifying activation products in complex samples. Quantitative use of such data—e.g., absolute transition intensities and level lifetimes—critically depends on (i) accurate full-energy peak (FEP) efficiency determination and (ii) precise correction for true-coincidence summing (TCS). 
info.ornl.gov
+1
In the compact, high-efficiency geometries favored by modern systems—close source-to-detector distances, collimation, and extensive shielding—analytic efficiency models are typically insufficient. Monte Carlo (MC) transport has therefore become the standard for modeling γ-ray interactions and detector response in such environments. Among available toolkits, Geant4 is widely adopted for HPGe efficiency calibration and TCS analysis in complex assemblies. 
SpringerLink
+1
At the Dalat Nuclear Research Institute (Dalat NRI, Vietnam), a dedicated γ–γ coincidence spectrometer comprising two coaxial HPGe detectors is installed on the tangential thermal-neutron beamline of the Dalat Nuclear Research Reactor. The system is used routinely for neutron-capture and activation studies—mapping level schemes, characterizing γ–γ cascades, and searching for anomalous decay patterns—continuing a local line of development of coincidence instrumentation. To exploit the instrument fully and ensure metrological rigor, we require an experiment-specific MC model that reproduces absolute and relative photopeak efficiencies in singles and, crucially, geometry-dependent coincidence efficiencies and summing probabilities. 
Academia
+1
This work presents the development and validation of a detailed Geant4 model of the Dalat HPGe coincidence system. The setup poses nontrivial modeling challenges: a compact detector arrangement; thick, composite shielding (lead and borated polyethylene) for neutron suppression; and experiment-specific data-acquisition (DAQ) logic for coincidence processing. Capturing these elements faithfully is essential to obtain a reliable “digital twin” of the spectrometer.
For validation we employ the well-characterized ^35Cl(n,γ)^36Cl reaction as a benchmark. The ^36Cl decay scheme is firmly established, and its intense, well-separated γ–γ cascades provide an ideal set of lines spanning a broad energy range. Experimentally, ^36Cl activity is produced via in-reactor irradiation and recorded in both singles and coincidence modes. To concentrate computational effort on detector response and electronic logic, our simulations omit neutron transport and instead generate γ-rays directly from the excited ^36Cl cascade as the primary source. 
APS Link
+1
Validation is multi-faceted, comparing experiment and simulation across: (i) absolute and relative FEP efficiencies in singles; (ii) true-coincidence efficiencies for selected cascade pairs; (iii) the magnitude of summing-in/summing-out effects and peak-to-total behavior; (iv) spectral shapes in coincidence-gated spectra; and (v) timing distributions between detectors. We also quantify the impact of uncertainties (geometric tolerances, material compositions, and nuclear-data inputs) on the agreement.
The successful validation establishes the Geant4 model as a reliable tool at Dalat NRI for: (1) efficiency calibrations over a broad energy range; (2) accurate TCS corrections in quantitative analysis; and (3) design and feasibility assessment of future measurements. While the present paper focuses on methodology and validation with the ^35Cl(n,γ)^36Cl benchmark, the model underpins a wider research program, including studies of isotopes such as ^54Cr, where resolving incomplete level schemes, identifying new candidates, and interpreting anomalous cascade intensities require a precisely characterized detector response.
In what follows, we describe the experimental setup and DAQ system, detail the Geant4 geometry and physics lists, and explain the emulation of the coincidence logic. We then compare simulated and measured observables, discuss residual discrepancies, and provide an uncertainty budget, before concluding with prospects for applying this validated framework to advanced nuclear-structure and activation studies at Dalat NRI.

Why these citations?
Coincidence advantages & practical workflow for HPGe systems, incl. gating, SNR, and background suppression: ORNL tech report and recent NIM-A discussion of γ–γ analysis and summing. 
info.ornl.gov
+1
Geant4 as standard for HPGe efficiency and TCS in complex geometries (recent applications in well and clover detectors). 
SpringerLink
+1
Benchmark choice and maturity of the ^35Cl(n,γ)^36Cl decay scheme (classic γ–γ capture study and a high-precision level-scheme paper). 
APS Link
+1
Dalat γ–γ spectrometer context and local track record. 
Academia
+1

## 2. Experimental Setup and Data Acquisition

### 2.1 Neutron Beam and Irradiation Geometry
- Description of the Dalat Nuclear Research Reactor tangential thermal-neutron beamline used for experiments.
- Beam conditioning: 48 cm single-crystal Si filter and paraffin–boron collimator to enhance thermal flux and suppress fast neutrons.
- Thermal beam characteristics at the target: diameter (~2.5 cm) and flux (~1.7 × 10⁵ n·cm⁻²·s⁻¹).
- Target preparation and positioning: targets sealed in plastic, located at beam center; justification for geometry choice regarding summing and efficiency.
- Operational constraints: choice of narrower collimator and longer filter to reduce fast-neutron background and electronics dead time (~2.3%).

### 2.2 γ–γ Coincidence Detector System
- Description of the two ORTEC coaxial HPGe detectors (35% relative efficiency) and mechanical arrangement.
- Geometry: target-to-detector distance (~5 cm), detector orientation (facing target along beam axis), and symmetry of configuration.
- Shielding and background reduction: Pb plates, lead cone collimators, Cu/LiF fills, and B4C shields; purpose and expected impact on background and detector damage.
- Use of Cl-36 as a standard efficiency calibration source for the coincidence system.

### 2.3 Data Acquisition Electronics and Coincidence Logic
- Overview of the digital data acquisition chain used for γ–γ coincidence measurements.
- Implementation of coincidence logic: event building and time-stamping; coincidence time window (~100 ns) and energy thresholds (~0.05 MeV per channel).
- Calibration procedures: energy calibration, timing alignment between channels, and efficiency calibration using Cl-36.
- Treatment of dead time, pileup, and baseline restoration (brief description aligned with available details).
- Data products: list-mode or event files storing paired energies (and times) for coincident events, later sorted into two-step cascades.

## 3. Geant4 Model of the Coincidence Detector System

### 3.1 Geometry and Materials
- Summary of the Geant4 world geometry corresponding to the experimental setup (HPGe crystals, endcaps, cryostats, shielding, collimators, and support structures).
- Modeling of detector crystal dimensions, dead layers, and spacing, based on manufacturer data and additional measurements/assumptions.
- Representation of beamline components and shielding (Si filter, paraffin–boron collimator, Pb, LiF, B4C, etc.) to the extent needed for γ-transport.
- Discussion of geometric simplifications versus the real setup and their estimated impact on efficiency and coincidence observables.

### 3.2 Physics Lists and Nuclear Data for Cl-36
- Choice of Geant4 version and electromagnetic physics list used for γ-ray transport and interactions.
- Approach to modeling Cl-36 de-excitation: use of evaluated nuclear data (e.g. ENSDF-based level scheme) and implementation in Geant4 (built-in decay modules or custom generator).
- Description of the Cl-35(n,γ)Cl-36 cascade scheme, focusing on the subset of levels and transitions relevant for the benchmark.
- Handling of angular correlations and cascade ordering if implemented, or justification if neglected.

### 3.3 Source Definition and Modeling Approximation
- Explanation of the modeling strategy: omit explicit neutron transport and use excited Cl-36 as the primary radiation source.
- Justification for starting from excited Cl-36 (computational efficiency, focus on detector response and coincidence logic).
- Description of how emission positions and directions are sampled (target volume, isotropic emission, etc.).
- Discussion of the limitations introduced by this approximation and potential biases versus a full neutron-transport model.

### 3.4 Electronics Response and Coincidence Emulation
- Modeling of energy resolution and thresholds: application of detector response functions (e.g. Gaussian smearing) to Geant4 energy deposits.
- Event and hit processing: conversion of Geant4 hits into simulated detector “events” for each HPGe, including summing within a time window if applicable.
- Implementation of coincidence logic at analysis level to mirror the experimental 100 ns time window and low-energy cutoffs.
- Optional modeling of timing jitter, dead time, and pileup if included; otherwise, qualitative discussion and rationale for neglecting them.

## 4. Validation with Cl-36 Cascade Gamma Data

### 4.1 Experimental Data Selection and Analysis
- Description of the Cl-36 data set: acquisition conditions, counting statistics, and relevant γ-ray lines and cascade pairs.
- Event-building and coincidence sorting procedures in the experimental analysis, including energy and timing cuts.
- Construction of singles spectra and coincidence-gated spectra (e.g. gating on specific transitions or energy windows).

### 4.2 Comparison of Singles Efficiencies
- Extraction of experimental full-energy peak efficiencies as a function of energy using Cl-36 lines.
- Extraction of simulated efficiencies from Geant4 for the same γ-ray energies and geometry.
- Presentation of experiment vs. simulation efficiency curves, with percent differences and χ² or residuals.
- Discussion of main contributors to discrepancies: geometry tolerances, nuclear data, resolution modeling, etc.

### 4.3 Coincidence Efficiencies and Summing Effects
- Definition and extraction of coincidence efficiencies and/or coincidence-summing probabilities for selected cascade pairs.
- Comparison of experimental and simulated coincidence efficiencies and coincidence-summing observables.
- Analysis of how shielding, geometry, and thresholds affect coincidence-summing and peak intensities.

### 4.4 Gated Spectra and Timing Distributions
- Comparison of experimental and simulated energy spectra under specific coincidence gates (e.g. gating on a partner transition).
- Analysis of timing-distribution comparisons between detectors, including coincidence peak width and possible background components.
- Quantitative and qualitative evaluation of how well the timing and gating behavior is reproduced by the simulation.

### 4.5 Uncertainty and Sensitivity Analysis
- Summary of experimental uncertainties: counting statistics, calibration, dead-time corrections, neutron flux, and target positioning.
- Summary of simulation uncertainties: geometry tolerances (distances, crystal dimensions), material compositions, and nuclear data choices.
- Sensitivity checks: impact of small geometry changes or resolution parameters on simulated efficiencies and coincidences.
- Overall assessment of agreement between experiment and simulation, including systematic trends.

## 5. Results and Discussion
- Synthesis of validation results: how well the Geant4 model reproduces Cl-36 singles and coincidence observables across energy, angle, and gates.
- Interpretation of residual discrepancies in terms of geometry, physics modeling, or DAQ approximations.
- Assessment of the reliability and domain of applicability of the simulation for efficiency and coincidence-summing corrections.
- Comparison to similar validation efforts in the literature (other Geant4-based coincidence or HPGe-efficiency studies).

## 6. Outlook and Applications to Other Isotopes
- Role of the validated Geant4 model as a tool for future studies at Dalat NRI.
- Planned applications to other neutron-capture or activation products (e.g. Cr-54 and additional isotopes) for level-scheme studies and anomaly detection.
- Use of the simulation to support new level detection, abnormal cascade behavior identification, and improved efficiency/summing corrections.
- Clarification that detailed Cr-54 physics results will be presented in a separate paper, which will reference the present validation work.

## 7. Conclusions
- Concise summary of the development of a detailed Geant4 model for the Dalat HPGe coincidence detector system.
- Statement on the level of agreement between simulated and measured Cl-36 singles and coincidence data and what this implies for model validity.
- Key lessons on modeling approximations (excited Cl-36 source, DAQ emulation) and their impact.
- Final remarks on broader relevance of the validated model and future work directions.

