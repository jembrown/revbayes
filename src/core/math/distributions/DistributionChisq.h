/**
 * @file DistributionChisq
 * This file contains the functions of the chi-square distribution.
 *
 * @brief Implementation of the chi-square distribution.
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes core development team
 * @license GPL version 3
 * @version 1.0
 * @since 2011-03-17, version 1.0
 *
 * $Id$
 */


#ifndef DistributionChisq_H
#define DistributionChisq_H

namespace RevBayesCore {
    
    class RandomNumberGenerator;

    namespace RbStatistics {
    
        namespace ChiSquare {
        
            double                      pdf(double v, double x);                                              /*!< Chi-Square(v) probability density */
            double                      lnPdf(double v, double x);                                            /*!< Chi-Square(v) log_e probability density */
            double                      cdf(double v, double x);                                              /*!< Chi-Square(v) cumulative probability */
            double                      quantile(double prob, double v);                                         /*!< Chi-Square(v) quantile */
            double                      rv(double v, RandomNumberGenerator& rng);                             /*!< Chi-Square(v) random variable */
        }
    }
}

#endif
