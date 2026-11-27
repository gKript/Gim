//
//  gim_ai_train.h
//
//  Gim - Generic Information Manager
//
//  [http://www.gkript.org]
//
/*#############################################################################

	Copyright (C) 2002-2026
	Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)

	This file is part of Gim library.

	Gim is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	Gim is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Gim.  If not, see <https://www.gnu.org/licenses/>.

#############################################################################*/
//
/*!     \file       gim_ai_train.h
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/
/*!	\file		gim_ai_train.h
	\brief		Gim side of the PicGIM AI model (SYNC/COMM_CONTRACT.md, sez. 4.4 - 4.6).
				Trains a small fully connected network on a labelled dataset of raw sensor samples,
				quantizes it to the integer format of pgim_ai_net.c, checks it with a bit-exact
				integer simulator and, only if accepted, produces pgim_ai_model.h.

				Port of scripts/gim_ai_train.py (same algorithm, same checks).

	Released under the terms of the GNU General Public License v3, as the rest of Gim.
*/

#ifndef _GIM_AI_TRAIN_H_
#define _GIM_AI_TRAIN_H_

	#include <cstdint>
	#include <stdexcept>
	#include <string>
	#include <vector>

	namespace gim_ai {

		/*! Error raised when a model cannot pass a contract check.
			It is not an input-format error (those are std::invalid_argument).
		*/
		class ModelError : public std::runtime_error {
			public:
				/*! Builds the error with a message.
					\param what	description of the problem
				*/
				explicit ModelError( const std::string & what ) : std::runtime_error( what ) {}
		};

		/*! One dataset row: raw value and status per channel, plus the class label.
		*/
		struct Sample {
			std::vector<uint16_t>	raw;	/*!< raw value of each channel */
			std::vector<uint8_t>	ok;		/*!< status of each channel: 1 = PG_OK, 0 = sensor fault */
			int						label;	/*!< class label */
		};

		/*! A labelled dataset read from CSV.
		*/
		struct Dataset {
			std::vector<std::string>	names;		/*!< channel names */
			std::vector<Sample>			samples;	/*!< all the rows */
			int							n_classes;	/*!< number of classes */
			std::string					sha256;		/*!< hex digest of the dataset file */
		};

		/*! Training and quantization options, same names and defaults as the Python script.
		*/
		struct Options {
			int			out_bits;		/*!< output bits of the quantized model: 8 or 16 */
			std::vector<int> hidden;	/*!< sizes of the hidden layers */
			int			epochs;			/*!< training epochs */
			double		lr;				/*!< learning rate */
			double		test_frac;		/*!< fraction of the samples used for testing */
			uint32_t	seed;			/*!< seed of the random generator */
			double		tol;			/*!< max accuracy drop of the integer model against the float one */
			int			test_vectors;	/*!< number of test vectors written in the header */
			/*! Builds the options with the default values.
			*/
			Options() : out_bits( 8 ), hidden( 1 , 8 ), epochs( 200 ), lr( 0.05 ), test_frac( 0.25 ),
						seed( 1 ), tol( 0.02 ), test_vectors( 16 ) {}
		};

		/*! Result of a pipeline run.
			header is empty when the model is not accepted.
		*/
		struct Outcome {
			bool		accepted;	/*!< true when the integer model passes the checks */
			double		float_acc;	/*!< accuracy of the float model */
			double		int_acc;	/*!< accuracy of the integer model */
			std::string	report;		/*!< text report of the run */
			std::string	header;		/*!< content of pgim_ai_model.h, empty when not accepted */
		};

		/*! Reads the CSV dataset (header: label,ch0,ch0_ok,ch1,ch1_ok,...).
			\param path	path of the CSV file
			\return		the dataset
			\exception	std::invalid_argument on bad input
		*/
		Dataset read_dataset( const std::string & path );

		/*! Trains, quantizes, checks and builds pgim_ai_model.h.
			\param ds	dataset to train on
			\param opt	options of the run
			\return		the outcome of the run
			\exception	ModelError for constructions that cannot be made
		*/
		Outcome run_pipeline( const Dataset & ds , const Options & opt );

		/*! Writes a synthetic dataset for testing (3 classes, 8 channels).
			\param path	path of the CSV file to write
			\param n	number of rows
			\param seed	seed of the random generator
		*/
		void write_synthetic( const std::string & path , int n , uint32_t seed );

		/*! Computes the SHA-256 digest of a byte string (uses gim_sha256_obj).
			\param bytes	the data
			\return		the digest as hexadecimal text
		*/
		std::string sha256_hex( const std::string & bytes );

	}	// namespace gim_ai

#endif
