# Geant4 Simulation of the HPGe Coincidence Detector System at Dalat NRI – Paper Outline

## 1. Introduction
High-purity germanium (HPGe) γ–γ coincidence spectrometers at research reactors are indispensable tools for high-resolution nuclear spectroscopy and nuclear-data production. By enforcing time-correlated detection of cascade γ-rays, coincidence techniques disentangle complex level schemes, strongly suppress uncorrelated background, and reveal weak branches that are easily hidden in singles spectra. Over the past decade, such measurements at the Dalat Nuclear Research Reactor (DNRR) have underpinned detailed nuclear-structure studies, including γ-cascade spectroscopy of the ^51V(n_th,γ)^52V reaction, determinations of level densities and γ-strength functions in isotopes such as ^172Yb and ^153Sm, and comprehensive level-scheme work on nuclei including ^153Sm and ^182Ta, with several results published in leading journals and recommended for updating evaluated databases. These achievements demonstrate the scientific potential of γ–γ coincidence spectroscopy at DNRR, but they also place stringent demands on the metrological quality of the detector characterization.
Quantitative exploitation of coincidence data—e.g., extraction of absolute transition intensities, reliable branching ratios, and, ultimately, observables such as angular correlations—critically depends on (i) accurate full-energy peak (FEP) efficiency determination for each detector and (ii) precise correction for true-coincidence summing (TCS) in both singles and coincidence modes. In modern, compact high-efficiency geometries, characterized by short source-to-detector distances, collimation, and extensive shielding, simple analytical efficiency models and source-based calibrations are generally inadequate, especially when one targets weak cascades or highly converted transitions. For the Dalat γ–γ spectrometer, which is used routinely with in-beam irradiations and complex activation samples, an experiment-specific, high-fidelity response model is therefore essential to ensure the reliability and reproducibility of the derived nuclear-structure results.
Monte Carlo (MC) radiation transport has become the standard approach for modeling γ-ray interactions and detector response in such environments. Among available toolkits, Geant4 is widely adopted for HPGe efficiency calibrations and TCS analyses in complex geometries. Most published applications, however, focus on singles HPGe detectors or relatively simple coincidence arrangements and only approximate the details of data-acquisition (DAQ) electronics. To our knowledge, no comprehensive Geant4-based simulation has previously been developed for the dedicated double-HPGe γ–γ coincidence spectrometer installed on the tangential thermal-neutron beamline of the DNRR. More broadly, there is no published study that models, in a single framework, (i) a double HPGe coincidence system coupled to a reactor neutron beam, (ii) cascade γ-rays from neutron-capture reactions, and (iii) the full digital coincidence logic of a custom DAQ chain. This gap limits the quantitative reach of existing coincidence measurements and constrains the design of future experiments.
At the Dalat Nuclear Research Institute (Dalat NRI, Vietnam), the γ–γ coincidence spectrometer consists of two coaxial HPGe detectors operated in close geometry, surrounded by thick, composite shielding (lead and borated polyethylene) for neutron suppression and coupled to a digitally implemented coincidence DAQ. The system has been optimized for (n,2γ) studies and activation measurements, and it forms the core of an ongoing program aimed at extending level schemes, identifying new transitions, and probing anomalous cascade behavior in nuclei such as ^54Cr and several medium-heavy isotopes. To fully exploit this instrument and move toward more demanding observables, including angular-correlation measurements of cascade γ-rays, we require a detailed, experiment-specific Geant4 model that can serve as a reliable “digital twin” of the spectrometer, reproducing absolute and relative photopeak efficiencies, coincidence efficiencies, and summing probabilities under realistic operating conditions.
In this work we present the development and validation of such a Geant4 model for the Dalat HPGe γ–γ coincidence system. The simulation includes detailed descriptions of the detector crystals and cryostats, sample and holder geometry, shielding and surrounding materials, and an explicit emulation of the custom digital DAQ logic used for coincidence processing. For validation we employ the well-characterized ^35Cl(n,γ)^36Cl reaction as a benchmark. The ^36Cl decay scheme is firmly established, and its intense, well-separated γ–γ cascades provide an ideal set of lines spanning a broad energy range for both singles and coincidence efficiency tests. Experimentally, ^36Cl activity is produced via in-reactor irradiation and recorded simultaneously in singles and coincidence modes. In the simulation, to concentrate computational effort on detector response and electronic logic, we omit explicit neutron transport and instead generate γ-rays directly from the excited ^36Cl cascade as the primary source, a deliberate approximation that preserves the relevant cascade kinematics while simplifying the transport problem.
The validation strategy is multi-faceted. We compare experiment and simulation for (i) absolute and relative FEP efficiencies in singles; (ii) true-coincidence efficiencies for selected cascade pairs; (iii) the magnitude of summing-in/summing-out effects and peak-to-total behavior; (iv) spectral shapes in coincidence-gated spectra; and (v) timing distributions between detectors. We also investigate the impact of uncertainties in geometry, material composition, and nuclear-data inputs on the agreement, thereby quantifying the robustness of the model for routine use. The successful validation establishes the Geant4 model as a reliable tool at Dalat NRI for efficiency calibration over a broad energy range, accurate TCS corrections in quantitative analysis, and design and feasibility assessments of future measurements.
The purpose of this paper is therefore threefold: (1) to develop a detailed Geant4 “digital twin” of the Dalat HPGe γ–γ coincidence spectrometer, including geometry, shielding, and custom DAQ logic; (2) to validate this model comprehensively using ^35Cl(n,γ)^36Cl data in both singles and coincidence modes; and (3) to establish an experiment-specific simulation framework that underpins a broader program of neutron-capture and activation studies at DNRR, including future angular-correlation measurements and investigations of isotopes such as ^54Cr. In what follows, we describe the experimental setup and DAQ system, detail the Geant4 geometry and physics lists, and explain the emulation of the coincidence logic. We then compare simulated and measured observables, discuss residual discrepancies and associated uncertainties, and conclude with prospects for applying this validated framework to advanced nuclear-structure and activation studies at Dalat NRI.
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
