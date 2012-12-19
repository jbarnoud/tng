/** @file tng_io.h
 *  @brief API for input and output of tng trajectory files
 *  @mainpage TNG: A flexible binary trajectory format
 *  @section intro_sec Introduction
 *
 * The TNG format is developed as part of the ScalaLife EU project.
 * It is flexible by design to allow parallel writing, custom data blocks,
 * different output frequencies and different compression algorithms.
 *
 * Each block can contain MD5 hashes to verify data integrity and the file
 * can be signed by the user to ensure that the origin is correct.
 *
 * @section authors_sec Authors
 * 
 * The TNG trajectory format is developed by:
 * 
 * Magnus Lundborg magnus.lundborg@scilifelab.se
 *
 * Daniel Spångberg daniels@mkem.uu.se
 *
 * Rossen Apostolov rossen@kth.se
 *
 * The API is implemented mainly by:
 *
 * Magnus Lundborg
 *
 * @section License
 *
 * The TNG API is released under LGPL 2.1 and is free to redistribute according
 * to that license (or a later version of the LGPL license).
 *
 * A license file (named COPYING) should be included with each copy of the API.
 *
 * @section install_sec Installation
 *
 * mkdir build
 * 
 * cd build
 * 
 * cmake ..
 * 
 * make
 * 
 *
 * Test by running:
 * 
 * bin/tng_testing
 * 
 */

#ifndef _TNGIO_H
#define _TNGIO_H     1

#include <stdio.h>
#include <inttypes.h>

/** The version of this TNG build */
#define TNG_VERSION 1

/** Flag to indicate particle dependent data. */
#define TNG_PARTICLE_DEPENDENT 1
/** Flag to indicate frame dependent data. */
#define TNG_FRAME_DEPENDENT 2

/** The maximum length of a date string */
#define TNG_MAX_DATE_STR_LEN 24
/** The length of an MD5 hash */
#define TNG_HASH_LEN 16
/** The maximum allowed length of a string */
#define TNG_MAX_STR_LEN 1024


/** Inline function for finding the lowest of two values */
#define min(a,b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a < _b ? _a : _b; })
     
/** Inline function for finding the highest of two values */
#define max(a,b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a > _b ? _a : _b; })


typedef enum {TNG_BIG_ENDIAN_32,
              TNG_LITTLE_ENDIAN_32,
              TNG_BYTE_PAIR_SWAP_32} tng_endianness_32;

typedef enum {TNG_BIG_ENDIAN_64,
              TNG_LITTLE_ENDIAN_64,
              TNG_QUAD_SWAP_64,
              TNG_BYTE_PAIR_SWAP_64,
              TNG_BYTE_SWAP_64} tng_endianness_64;


typedef enum {TNG_UNCOMPRESSED,
              TNG_XTC_COMPRESSION,
              TNG_TNG_COMPRESSION} tng_compression;

typedef enum {TNG_NON_TRAJECTORY_BLOCK, TNG_TRAJECTORY_BLOCK} tng_block_type;

typedef enum {TNG_ENDIANNESS_AND_STRING_LENGTH,
              TNG_GENERAL_INFO,
              TNG_MOLECULES,
              TNG_TRAJECTORY_IDS_AND_NAMES,
              TNG_TRAJECTORY_FRAME_SET,
              TNG_BLOCK_TABLE_OF_CONTENTS,
              TNG_PARTICLE_MAPPING} tng_non_trajectory_block_ids;

typedef enum {TNG_TRAJ_BOX_SHAPE = 10000,
              TNG_TRAJ_POSITIONS,
              TNG_TRAJ_VELOCITIES,
              TNG_TRAJ_FORCES} tng_trajectory_block_ids;
              

typedef enum {TNG_NON_PARTICLE_BLOCK_DATA,
              TNG_PARTICLE_BLOCK_DATA} tng_particle_block_data;
              
/*typedef enum {TNG_NO_HASH, TNG_OTHER_HASH,
              TNG_MD5_HASH, TNG_MD6_HASH,
              TNG_SHA0_HASH, TNG_SHA1_HASH,
              TNG_SHA256_HASH, TNG_SHA512_HASH} tng_hash_type;*/

typedef enum {FALSE, TRUE} tng_bool;

typedef enum {TNG_CONSTANT_N_ATOMS, TNG_VARIABLE_N_ATOMS}
             tng_variable_n_atoms_flag;

typedef enum {TNG_SUCCESS, TNG_FAILURE, TNG_CRITICAL} tng_function_status;

typedef enum {TNG_SKIP_HASH, TNG_USE_HASH} tng_hash_mode;

typedef enum {TNG_CHAR_DATA,
              TNG_INT_DATA,
              TNG_FLOAT_DATA,
              TNG_DOUBLE_DATA} tng_data_type;


struct tng_trajectory;
struct tng_molecule;
struct tng_chain;
struct tng_residue;
struct tng_atom;
struct tng_bond;
struct tng_gen_block;
struct tng_frame_set_toc;
struct tng_particle_mapping;
struct tng_trajectory_frame_set;
struct tng_particle_data;
struct tng_non_particle_data;

typedef struct tng_trajectory *tng_trajectory_t;
typedef struct tng_molecule *tng_molecule_t;
typedef struct tng_chain *tng_chain_t;
typedef struct tng_residue *tng_residue_t;
typedef struct tng_atom *tng_atom_t;
typedef struct tng_bond *tng_bond_t;
typedef struct tng_gen_block *tng_gen_block_t;
typedef struct tng_frame_set_toc *tng_frame_set_toc_t;
typedef struct tng_particle_mapping *tng_particle_mapping_t;
typedef struct tng_trajectory_frame_set *tng_trajectory_frame_set_t;
typedef struct tng_particle_data *tng_particle_data_t;
typedef struct tng_non_particle_data *tng_non_particle_data_t;

/** Data can be either double, float, int or a string */
union data_values {
    double d;
    float f;
    int i;
    char *c;
};


#ifdef __cplusplus
extern "C"
{
#endif



/**
 * @brief Setup a trajectory data container.
 * @param tng_data_p a pointer to memory to initialise as a trajectory.
 * @details Memory is allocated during initialisation.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_trajectory_init(tng_trajectory_t *tng_data_p);

/**
 * @brief Clean up a trajectory data container.
 * @param tng_data_p a pointer to the trajectory data to destroy.
 * @details All allocated memory in the data structure is freed, as well as
 * tng_data_p itself.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_trajectory_destroy(tng_trajectory_t *tng_data_p);

/**
 * @brief Set the name of the input file.
 * @param tng_data the trajectory of which to set the input file name.
 * @param file_name the name of the input file.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_input_file_set(tng_trajectory_t tng_data,
                                       const char *file_name);

/**
 * @brief Set the name of the output file.
 * @param tng_data the trajectory of which to set the output file name.
 * @param file_name the name of the output file.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_output_file_set(tng_trajectory_t tng_data,
                                        const char *file_name);

/**
 * @brief Set the name of the program used when creating the trajectory.
 * @param tng_data the trajectory of which to set the program name.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_first_program_name_set(tng_trajectory_t tng_data,
                                               const char *new_name);

/**
 * @brief Set the name of the program used when last modifying the trajectory.
 * @param tng_data the trajectory of which to set the program name.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_last_program_name_set(tng_trajectory_t tng_data,
                                              const char *new_name);

/**
 * @brief Set the name of the user who created the trajectory.
 * @param tng_data the trajectory of which to set the user name.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_first_user_name_set(tng_trajectory_t tng_data,
                                            const char *new_name);

/**
 * @brief Set the name of the user who last modified the trajectory.
 * @param tng_data the trajectory of which to set the user name.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_last_user_name_set(tng_trajectory_t tng_data,
                                           const char *new_name);

/**
 * @brief Set the name of the computer used when creating the trajectory.
 * @param tng_data the trajectory of which to set the computer name.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_first_computer_name_set(tng_trajectory_t tng_data,
                                                const char *new_name);

/**
 * @brief Set the name of the computer used when last modifying the trajectory.
 * @param tng_data the trajectory of which to set the computer name.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_last_computer_name_set(tng_trajectory_t tng_data,
                                               const char *new_name);

/**
 * @brief Set the pgp_signature of the user creating the trajectory.
 * @param tng_data the trajectory of which to set the computer name.
 * @param signature is a string containing the pgp_signature.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_first_signature_set(tng_trajectory_t tng_data,
                                            const char *signature);

/**
 * @brief Set the pgp_signature of the user last modifying the trajectory.
 * @param tng_data the trajectory of which to set the computer name.
 * @param signature is a string containing the pgp_signature.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_last_signature_set(tng_trajectory_t tng_data,
                                           const char *signature);

/**
 * @brief Set the name of the forcefield used in the trajectory.
 * @param tng_data the trajectory of which to set the forcefield name.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_forcefield_name_set(tng_trajectory_t tng_data,
                                            const char *new_name);

/**
 * @brief Get the medium stride length of the trajectory.
 * @param tng_data is the trajectory from which to get the stride length.
 * @param len is pointing to a value set to the stride length.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_medium_stride_length_get(tng_trajectory_t tng_data,
                                               int64_t *len);

/**
 * @brief Set the medium stride length of the trajectory.
 * @param tng_data is the trajectory of which to set the stride length.
 * @param len is the wanted medium stride length.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_medium_stride_length_set(tng_trajectory_t tng_data,
                                               int64_t len);

/**
 * @brief Get the long stride length of the trajectory.
 * @param tng_data is the trajectory from which to get the stride length.
 * @param len is pointing to a value set to the stride length.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_long_stride_length_get(tng_trajectory_t tng_data,
                                               int64_t *len);

/**
 * @brief Set the long stride length of the trajectory.
 * @param tng_data is the trajectory of which to set the stride length.
 * @param len is the wanted long stride length.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_long_stride_length_set(tng_trajectory_t tng_data,
                                               int64_t len);

/**
 * @brief Get the reading position of the input file.
 * @param tng_data is the trajectory from which to get the position.
 * @param pos is pointing to a value set to the reading position.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_input_file_pos_get(tng_trajectory_t tng_data,
                                           int64_t *pos);

/**
 * @brief Get the writing position of the output file.
 * @param tng_data is the trajectory from which to get the position.
 * @param pos is pointing to a value set to the writing position.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_output_file_pos_get(tng_trajectory_t tng_data,
                                            int64_t *pos);

/**
 * @brief Get the length of the input file.
 * @param tng_data is the trajectory from which to get the input file length.
 * @param len is pointing to a value set to the file length.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_input_file_len_get(tng_trajectory_t tng_data,
                                           int64_t *len);

/**
 * @brief Get the current number of particles.
 * @param tng_data is the trajectory from which to get the number of particles.
 * @param n is pointing to a value set to the number of particles.
 * @details If variable number of particles are used this function will return
 * the number of particles in the current frame set.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_num_particles_get(tng_trajectory_t tng_data,
                                          int64_t *n);

/**
 * @brief Get the current total number of molecules.
 * @param tng_data is the trajectory from which to get the number of molecules.
 * @param n is pointing to a value set to the number of molecules.
 * @details If variable number of particles are used this function will return
 * the total number of molecules in the current frame set.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_num_molecules_get(tng_trajectory_t tng_data,
                                          int64_t *n);

/**
 * @brief Get the number of frames per frame set.
 * @param tng_data is the trajectory from which to get the number of frames
 * per frame set.
 * @param n is pointing to a value set to the number of frames per frame set.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_num_frames_per_frame_set_get(tng_trajectory_t tng_data,
                                                     int64_t *n);

/**
 * @brief Get the current trajectory frame set.
 * @param tng_data is the trajectory from which to get the frame set.
 * @param frame_set is pointing to the memory position of the frame set.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_current_frame_set_get
                (tng_trajectory_t tng_data,
                 tng_trajectory_frame_set_t frame_set);

/**
 * @brief Get the file position of the next frame set in the input file.
 * @param frame_set is the frame set of which to get the position of the
 * following frame set.
 * @param pos is pointing to a value set to the file position.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_frame_set_next_frame_set_file_pos_get
                (tng_trajectory_frame_set_t frame_set,
                 int64_t *pos);

/**
 * @brief Get the file position of the previous frame set in the input file.
 * @param frame_set is the frame set of which to get the position of the
 * previous frame set.
 * @param pos is pointing to a value set to the file position.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_frame_set_prev_frame_set_file_pos_get
                (tng_trajectory_frame_set_t frame_set,
                 int64_t *pos);

/**
 * @brief Setup a molecule container.
 * @param molecule is the molecule to initialise. Memory must be preallocated.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_molecule_init(tng_molecule_t molecule);

/**
 * @brief Clean up a molecule container.
 * @param molecule is the molecule to destroy.
 * @details All allocated memory in the data structure is freed, but not the
 * memory of molecule itself.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_molecule_destroy(tng_molecule_t molecule);


/**
 * @brief Add a molecule to the trajectory.
 * @param tng_data is the trajectory data container containing the block..
 * @param name is a pointer to the string containing the name of the new molecule.
 * @param molecule is a pointer to the newly created molecule.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_molecule_add(tng_trajectory_t tng_data,
                                     const char *name,
                                     tng_molecule_t *molecule);

/**
 * @brief Set the name of a molecule.
 * @param tng_data is the trajectory data container containing the molecule..
 * @param molecule is the molecule to rename.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_molecule_name_set(tng_trajectory_t tng_data,
                                          tng_molecule_t molecule,
                                          const char *new_name);

/**
 * @brief Get the count of a molecule.
 * @param tng_data is the trajectory data container containing the molecule..
 * @param molecule is the molecule of which to get the count.
 * @param cnt is a pointer to the variable to be populated with the count.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_molecule_cnt_get(tng_trajectory_t tng_data,
                                         tng_molecule_t molecule,
                                         int64_t *cnt);

/**
 * @brief Set the count of a molecule.
 * @param tng_data is the trajectory data container containing the molecule..
 * @param molecule is the molecule of which to set the count.
 * @param cnt is the number of instances of this molecule.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_molecule_cnt_set(tng_trajectory_t tng_data,
                                         tng_molecule_t molecule,
                                         int64_t cnt);

/**
 * @brief Add a chain to a molecule.
 * @param tng_data is the trajectory data container containing the molecule..
 * @param molecule is the molecule to add a chain to.
 * @param name is a string containing the name of the chain.
 * @param chain is a pointer to the newly created chain.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_molecule_chain_add(tng_trajectory_t tng_data,
                                              tng_molecule_t molecule,
                                              const char *name,
                                              tng_chain_t *chain);

/**
 * @brief Set the name of a chain.
 * @param tng_data is the trajectory data container containing the atom..
 * @param chain is the chain to rename.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_chain_name_set(tng_trajectory_t tng_data,
                                       tng_chain_t chain,
                                       const char *new_name);

/**
 * @brief Add a residue to a chain.
 * @param tng_data is the trajectory data container containing the chain..
 * @param chain is the chain to add a residue to.
 * @param name is a string containing the name of the residue.
 * @param residue is a pointer to the newly created residue.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_chain_residue_add(tng_trajectory_t tng_data,
                                             tng_chain_t chain,
                                             const char *name,
                                             tng_residue_t *residue);

/**
 * @brief Set the name of a residue.
 * @param tng_data is the trajectory data container containing the residue.
 * @param residue is the residue to rename.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_residue_name_set(tng_trajectory_t tng_data,
                                         tng_residue_t residue,
                                         const char *new_name);

/**
 * @brief Add an atom to a residue.
 * @param tng_data is the trajectory containing the residue.
 * @param residue is the residue to add an atom to.
 * @param atom_name is a string containing the name of the atom.
 * @param atom_type is a string containing the atom type of the atom.
 * @param atom is a pointer to the newly created atom.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_residue_atom_add(tng_trajectory_t tng_data,
                                            tng_residue_t residue,
                                            const char *atom_name,
                                            const char *atom_type,
                                            tng_atom_t *atom);

/**
 * @brief Set the name of an atom.
 * @param tng_data is the trajectory data container containing the atom.
 * @param atom is the atom to rename.
 * @param new_name is a string containing the wanted name.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_atom_name_set(tng_trajectory_t tng_data,
                                      tng_atom_t atom,
                                      const char *new_name);

/**
 * @brief Set the atom type of an atom.
 * @param tng_data is the trajectory data container containing the atom.
 * @param atom is the atom to change.
 * @param new_type is a string containing the atom type.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_atom_type_set(tng_trajectory_t tng_data,
                                      tng_atom_t atom,
                                      const char *new_type);

/**
 * @brief Add a particle mapping table.
 * @details Each particle mapping table will be written as a separate block,
 * followed by the data blocks for the corresponding particles. In most cases
 * there is one particle mapping block for each thread writing the trajectory.
 * @param tng_data is the trajectory, with the frame set to which to add
 * the mapping block.
 * @details The mapping information is added to the currently active frame set
 * of tng_data
 * @param first_particle_number is the first particle number of this mapping
 * block.
 * @param n_particles is the number of particles in this mapping block.
 * @param mapping_table is a list of the real particle numbers (i.e. the numbers
 * used in the molecular system). The list is n_particles long.
 * @details mapping_table[0] is the real particle number of the first particle
 * in the following data blocks.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_particle_mapping_add
                (tng_trajectory_t tng_data,
                 const int64_t first_particle_number,
                 const int64_t n_particles,
                 const int64_t *mapping_table);
                
/**
 * @brief Read the header blocks from the input_file of tng_data.
 * @details The trajectory blocks must be read separately and iteratively in chunks
 * to fit in memory.
 * @param tng_data is a trajectory data container.
 * @details tng_data->input_file_path specifies
 * which file to read from. If the file (input_file) is not open it will be
 * opened.
 * @param hash_mode is an option to decide whether to use the md5 hash or not.
 * If hash_mode == TNG_USE_HASH the written md5 hash in the file will be
 * compared to the md5 hash of the read contents to ensure valid data.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_file_headers_read(tng_trajectory_t tng_data,
                                          const tng_hash_mode hash_mode);

/**
 * @brief Write the header blocks to the output_file of tng_data.
 * @details The trajectory blocks must be written separately and iteratively in chunks
 * to fit in memory.
 * @param tng_data is a trajectory data container.
 * @details tng_data->output_file_path
 * specifies which file to write to. If the file (output_file) is not open it
 * will be opened.
 * @param hash_mode is an option to decide whether to use the md5 hash or not.
 * If hash_mode == TNG_USE_HASH an md5 hash for each header block will be generated.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_file_headers_write(tng_trajectory_t tng_data,
                                           const tng_hash_mode hash_mode);


/**
 * @brief Read one (the next) block (of any kind) from the input_file of tng_data.
 * @param tng_data is a trajectory data container.
 * @details tng_data->input_file_path specifies
 * which file to read from. If the file (input_file) is not open it will be
 * opened.
 * @param block_data is a pointer to the struct which will be populated with the
 * data.
 * @details If block_data->input_file_pos > 0 it is the position from where the
 * reading starts otherwise it starts from the current position.
 * @param hash_mode is an option to decide whether to use the md5 hash or not.
 * If hash_mode == TNG_USE_HASH the written md5 hash in the file will be
 * compared to the md5 hash of the read contents to ensure valid data.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_block_read_next(tng_trajectory_t tng_data,
                                        tng_gen_block_t block_data,
                                        const tng_hash_mode hash_mode);


/**
 * @brief Read one (the next) frame set, including toc, mapping and related data blocks
 * from the input_file of tng_data.
 * @param tng_data is a trajectory data container.
 * @details  tng_data->input_file_path specifies
 * which file to read from. If the file (input_file) is not open it will be
 * opened.
 * @param hash_mode is an option to decide whether to use the md5 hash or not.
 * If hash_mode == TNG_USE_HASH the written md5 hash in the file will be
 * compared to the md5 hash of the read contents to ensure valid data.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_frame_set_read_next(tng_trajectory_t tng_data,
                                            const tng_hash_mode hash_mode);

/**
 * @brief Write one frame set, including toc, mapping and related data blocks
 * to the output_file of tng_data.
 * @param tng_data is a trajectory data container.
 * @details  tng_data->output_file_path specifies
 * which file to write to. If the file (output_file) is not open it will be
 * opened.
 * @param hash_mode is an option to decide whether to use the md5 hash or not.
 * If hash_mode == TNG_USE_HASH an md5 hash for each header block will be generated.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_frame_set_write(tng_trajectory_t tng_data,
                                        const tng_hash_mode hash_mode);

/**
 * @brief Create and initialise a frame set.
 * @param tng_data is the trajectory data container in which to add the frame
 * set
 * @param first_frame is the first frame of the frame set.
 * @param n_frames is the number of frames in the frame set.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_frame_set_new(tng_trajectory_t tng_data,
                                      const int64_t first_frame,
                                      const int64_t n_frames);

/**
 * @brief Add a non-particle dependent data block.
 * @param tng_data is the trajectory data container in which to add the data
 * block
 * @param id is the block ID of the block to add.
 * @param block_name is a descriptive name of the block to add
 * @param datatype is the datatype of the data in the block (e.g. int/float)
 * @param block_type_flag indicates if this is a non-trajectory block (added
 * directly to tng_data) or if it is a trajectory block (added to the
 * frame set)
 * @param n_frames is the number of frames of the data block (automatically
 * set to 1 if adding a non-trajectory data block)
 * @param n_values_per_frame is how many values a stored each frame (e.g. 9
 * for a box shape block)
 * @param stride_length is how many frames are between each entry in the
 * data block
 * @param codec_id is the ID of the codec to compress the data.
 * @param new_data is an array of data values to add.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_data_block_add(tng_trajectory_t tng_data,
                                        const int64_t id,
                                        const char *block_name,
                                        const char datatype,
                                        const tng_block_type block_type_flag,
                                        int64_t n_frames,
                                        const int64_t n_values_per_frame,
                                        const int64_t stride_length,
                                        int64_t codec_id,
                                        void *new_data);
                                       

/**
 * @brief Add a particle dependent data block.
 * @param tng_data is the trajectory data container in which to add the data
 * block
 * @param id is the block ID of the block to add.
 * @param block_name is a descriptive name of the block to add
 * @param datatype is the datatype of the data in the block (e.g. int/float)
 * @param block_type_flag indicates if this is a non-trajectory block (added
 * directly to tng_data) or if it is a trajectory block (added to the
 * frame set)
 * @param n_frames is the number of frames of the data block (automatically
 * set to 1 if adding a non-trajectory data block)
 * @param n_values_per_frame is how many values a stored each frame (e.g. 9
 * for a box shape block)
 * @param stride_length is how many frames are between each entry in the
 * data block
 * @param first_particle_number is the number of the first particle stored
 * in this data block
 * @param n_particles is the number of particles stored in this data block
 * @param codec_id is the ID of the codec to compress the data.
 * @param new_data is an array of data values to add.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_particle_data_block_add(tng_trajectory_t tng_data,
                                        const int64_t id,
                                        const char *block_name,
                                        const char datatype,
                                        const tng_block_type block_type_flag,
                                        int64_t n_frames,
                                        const int64_t n_values_per_frame,
                                        const int64_t stride_length,
                                        const int64_t first_particle_number,
                                        const int64_t n_particles,
                                        int64_t codec_id,
                                        void *new_data);


/**
 * @brief Read a number of consecutive trajectory frames from the input_file of
 * tng_data.
 * @details Not implemented yet!
 * @param tng_data is a trajectory data container.
 * tng_data->input_file_path specifies
 * which file to read from. If the file (input_file) is not open it will be
 * opened.
 * @param start_frame_nr is the index number of the first frame to read.
 * @param end_frame_nr is the index number of the last frame to read.
 * @param hash_mode is an option to decide whether to use the md5 hash or not.
 * If hash_mode == TNG_USE_HASH the written md5 hash in the file will be
 * compared to the md5 hash of the read contents to ensure valid data.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_frame_read_interval(tng_trajectory_t tng_data,
                                       int64_t start_frame_nr,
                                       int64_t end_frame_nr,
                                       const tng_hash_mode hash_mode);

/**
 * @brief Write a number of consecutive trajectory frames to the output_file of
 * tng_data.
 * @details Not implemented yet!
 * @param tng_data is a trajectory data container. tng_data->output_file_path specifies
 * which file to write to. If the file (output_file) is not open it will be
 * opened.
 * @param start_frame_nr is the index number of the first frame to write.
 * @param end_frame_nr is the index number of the last frame to write.
 * @param hash_mode is an option to decide whether to use the md5 hash or not.
 * If hash_mode == TNG_USE_HASH the written md5 hash in the file will be
 * compared to the md5 hash of the read contents to ensure valid data.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_frame_write_interval(tng_trajectory_t tng_data,
                                        int64_t start_frame_nr,
                                        int64_t end_frame_nr,
                                        const tng_hash_mode hash_mode);

/**
 * @brief Free data is an array of values (2D).
 * @param values is the 2D array to free and will be set to 0 afterwards.
 * @param n_frames is the number of frames in the data array.
 * @param n_values_per_frame is the number of values per frame in the data array.
 * @param type is the data type of the data in the array (e.g. int/float/char).
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_data_values_free(union data_values **values,
                                         int64_t n_frames,
                                         int64_t n_values_per_frame,
                                         tng_data_type type);

/**
 * @brief Free data is an array of values (3D).
 * @param values is the array to free and will be set to 0 afterwards.
 * @param n_frames is the number of frames in the data array.
 * @param n_particles is the number of particles in the data array.
 * @param n_values_per_frame is the number of values per frame in the data array.
 * @param type is the data type of the data in the array (e.g. int/float/char).
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_particle_data_values_free(union data_values ***values,
                                                  int64_t n_frames,
                                                  int64_t n_particles,
                                                  int64_t n_values_per_frame,
                                                  tng_data_type type);

/**
 * @brief Retrieve non-particle data, from the last read frame set.
 * @param tng_data is a trajectory data container. tng_data->input_file_path specifies
 * which file to read from. If the file (input_file) is not open it will be
 * opened.
 * @param block_id is the id number of the particle data block to read.
 * @param values is a pointer to a 2-dimensional array (memory unallocated), which
 * will be filled with data. The array will be sized
 * (n_frames * n_values_per_frame).
 * Since ***values is allocated in this function it is the callers
 * responsibility to free the memory.
 * @param n_frames is set to the number of particles in the returned data. This is
 * needed to properly reach and/or free the data afterwards.
 * @param n_values_per_frame is set to the number of values per frame in the data.
 * This is needed to properly reach and/or free the data afterwards.
 * @param type is set to the data type of the data in the array.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_data_get(tng_trajectory_t tng_data,
                                 int64_t block_id,
                                 union data_values ***values,
                                 int64_t *n_frames,
                                 int64_t *n_values_per_frame,
                                 tng_data_type *type);

/**
 * @brief Read and retrieve non-particle data, in a specific interval.
 * @details Not implemented yet!
 * @param tng_data is a trajectory data container. tng_data->input_file_path specifies
 * which file to read from. If the file (input_file) is not open it will be
 * opened.
 * @param block_id is the id number of the particle data block to read.
 * @param start_frame_nr is the index number of the first frame to read.
 * @param end_frame_nr is the index number of the last frame to read.
 * @param values is a pointer to a 2-dimensional array (memory unallocated), which
 * will be filled with data. The array will be sized
 * (n_frames * n_values_per_frame).
 * Since ***values is allocated in this function it is the callers
 * responsibility to free the memory.
 * @param n_values_per_frame is set to the number of values per frame in the data.
 * This is needed to properly reach and/or free the data afterwards.
 * @param type is set to the data type of the data in the array.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_data_interval_get(tng_trajectory_t tng_data,
                                          int64_t block_id,
                                          int64_t start_frame_nr,
                                          int64_t end_frame_nr,
                                          union data_values ***values,
                                          int64_t *n_values_per_frame,
                                          tng_data_type *type);

/**
 * @brief Retrieve particle data, from the last read frame set.
 * @details The particle dimension of the returned values array is translated
 * to real particle numbering, i.e. the numbering of the actual molecular
 * system.
 * @param tng_data is a trajectory data container. tng_data->input_file_path
 * specifies which file to read from. If the file (input_file) is not open it
 * will be opened.
 * @param block_id is the id number of the particle data block to read.
 * @param values is a pointer to a 3-dimensional array (memory unallocated), which
 * will be filled with data. The array will be sized
 * (n_frames * n_particles * n_values_per_frame).
 * Since ****values is allocated in this function it is the callers
 * responsibility to free the memory.
 * @param n_frames is set to the number of particles in the returned data. This is
 * needed to properly reach and/or free the data afterwards.
 * @param n_particles is set to the number of particles in the returned data. This is
 * needed to properly reach and/or free the data afterwards.
 * @param n_values_per_frame is set to the number of values per frame in the data.
 * This is needed to properly reach and/or free the data afterwards.
 * @param type is set to the data type of the data in the array.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_particle_data_get(tng_trajectory_t tng_data,
                                          int64_t block_id,
                                          union data_values ****values,
                                          int64_t *n_frames,
                                          int64_t *n_particles,
                                          int64_t *n_values_per_frame,
                                          tng_data_type *type);

/**
 * @brief Read and retrieve particle data, in a specific interval.
 * @details Not implemented yet!
 * @details The particle dimension of the returned values array is translated
 * to real particle numbering, i.e. the numbering of the actual molecular
 * system.
 * @param tng_data is a trajectory data container. tng_data->input_file_path specifies
 * which file to read from. If the file (input_file) is not open it will be
 * opened.
 * @param block_id is the id number of the particle data block to read.
 * @param start_frame_nr is the index number of the first frame to read.
 * @param end_frame_nr is the index number of the last frame to read.
 * @param first_particle_number is the number of the first particle, for which to
 * retrieve data.
 * @param last_particle_number is the number of the last particle, for which to
 * retrieve data.
 * @param values is a pointer to a 3-dimensional array (memory unallocated), which
 * will be filled with data. The array will be sized
 * (n_frames * n_particles * n_values_per_frame).
 * Since ****values is allocated in this function it is the callers
 * responsibility to free the memory.
 * @param n_values_per_frame is set to the number of values per frame in the data.
 * This is needed to properly reach and/or free the data afterwards.
 * @param type is set to the data type of the data in the array.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_particle_data_interval_get(tng_trajectory_t tng_data,
                                                  int64_t block_id,
                                                  int64_t start_frame_nr,
                                                  int64_t end_frame_nr,
                                                  int64_t first_particle_number,
                                                  int64_t last_particle_number,
                                                  union data_values ****values,
                                                  int64_t *n_values_per_frame,
                                                  tng_data_type *type);

/** @brief Get the date and time of initial file creation in ISO format (string).
 *  @param tng_data is a trajectory data container.
 *  @param time is a pointer to the string in which the date will be stored. Memory
   must be reserved beforehand.
 * @return TNG_SUCCESS (0) if successful, TNG_FAILURE (1) if a minor error
 * has occurred or TNG_CRITICAL (2) if a major error has occured.
 */
tng_function_status tng_time_get_str(tng_trajectory_t tng_data, char *time);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif /* _TNGIO_H */