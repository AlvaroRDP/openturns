//                                               -*- C++ -*-
/**
 *  @brief Factory for Burr distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_BURRFACTORY_HXX
#define OPENTURNS_BURRFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Burr.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BurrFactory
 */
class OT_API BurrFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  BurrFactory();

  /** Virtual constructor */
  virtual BurrFactory * clone() const;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Implementation build(const NumericalSample & sample) const;
  Implementation build(const NumericalPoint & parameters) const;
  Implementation build() const;
  Burr buildAsBurr(const NumericalSample & sample) const;
  Burr buildAsBurr(const NumericalPoint & parameters) const;
  Burr buildAsBurr() const;

}; /* class BurrFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BURRFACTORY_HXX */