//                                               -*- C++ -*-
/**
 * @brief This class binds a Python object to an OpenTURNS' RandomVector
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "openturns/PythonRandomVectorImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Description.hxx"
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(PythonRandomVectorImplementation)

static const Factory<PythonRandomVectorImplementation> Factory_PythonRandomVectorImplementation;



/* Default constructor */
PythonRandomVectorImplementation::PythonRandomVectorImplementation()
  : RandomVectorImplementation(),
    pyObj_(0)
{
  // Nothing to do
}


/* Constructor from Python object*/
PythonRandomVectorImplementation::PythonRandomVectorImplementation(PyObject * pyObject)
  : RandomVectorImplementation(),
    pyObj_(pyObject)
{
  if ( !PyObject_HasAttrString( pyObj_, const_cast<char *>("getRealization") ) ) throw InvalidArgumentException(HERE) << "Error: the given object does not have a getRealization() method.";

  Py_XINCREF( pyObj_ );

  // Set the name of the object as its Python classname
  ScopedPyObjectPointer cls(PyObject_GetAttrString ( pyObj_,
                            const_cast<char *>( "__class__" ) ));
  ScopedPyObjectPointer name(PyObject_GetAttrString( cls.get(),
                             const_cast<char *>( "__name__" ) ));

  setName( checkAndConvert< _PyString_, String >(name.get()) );

  const UnsignedInteger dimension  = getDimension();
  Description description(dimension);
  ScopedPyObjectPointer desc(PyObject_CallMethod ( pyObj_,
                             const_cast<char *>( "getDescription" ),
                             const_cast<char *>( "()" ) ));
  if ( ( desc.get() != NULL )
       && PySequence_Check( desc.get() )
       && ( PySequence_Size( desc.get() ) == static_cast<SignedInteger>(dimension) ) )
  {
    description = convert< _PySequence_, Description >( desc.get() );
  }
  else for (UnsignedInteger i = 0; i < dimension; ++i) description[i] = (OSS() << "x" << i);
  setDescription(description);
}

/* Virtual constructor */
PythonRandomVectorImplementation * PythonRandomVectorImplementation::clone() const
{
  return new PythonRandomVectorImplementation(*this);
}

/* Copy constructor */
PythonRandomVectorImplementation::PythonRandomVectorImplementation(const PythonRandomVectorImplementation & other)
  : RandomVectorImplementation(other),
    pyObj_(other.pyObj_)
{
  Py_XINCREF( pyObj_ );
}

/* Destructor */
PythonRandomVectorImplementation::~PythonRandomVectorImplementation()
{
  Py_XDECREF( pyObj_ );
}

/* Comparison operator */
Bool PythonRandomVectorImplementation::operator ==(const PythonRandomVectorImplementation & ) const
{
  return true;
}

/* String converter */
String PythonRandomVectorImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << PythonRandomVectorImplementation::GetClassName()
      << " name=" << getName()
      << " description=" << getDescription();
  return oss;
}

/* String converter */
String PythonRandomVectorImplementation::__str__(const String & ) const
{
  OSS oss;
  oss << "class=" << PythonRandomVectorImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}


/* Here is the interface that all derived class must implement */


/* Accessor for input point dimension */
UnsignedInteger PythonRandomVectorImplementation::getDimension() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod ( pyObj_,
                               const_cast<char *>( "getDimension" ),
                               const_cast<char *>( "()" ) ));
  if ( result.isNull() )
  {
    handleException();
  }

  UnsignedInteger dim = convert< _PyInt_, UnsignedInteger >( result.get() );
  return dim;
}

Point PythonRandomVectorImplementation::getRealization() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod ( pyObj_,
                               const_cast<char *>( "getRealization" ),
                               const_cast<char *>( "()" ) ));
  if ( result.isNull() )
  {
    handleException();
  }
  Point point(convert<_PySequence_, Point>(result.get()));
  return point;
}


/* Numerical sample accessor */
Sample PythonRandomVectorImplementation::getSample(const UnsignedInteger size) const
{
  Sample sample;

  if ( PyObject_HasAttrString( pyObj_, const_cast<char *>("getSample") ) )
  {
    ScopedPyObjectPointer methodName(convert< String, _PyString_>( "getSample" ));
    ScopedPyObjectPointer sizeArg(convert< UnsignedInteger, _PyInt_ >(size));
    ScopedPyObjectPointer result(PyObject_CallMethodObjArgs( pyObj_,
                                 methodName.get(),
                                 sizeArg.get(), NULL ));
    if ( result.get() )
    {
      sample = convert<_PySequence_, Sample>(result.get());
      if (sample.getSize() != size) throw InvalidDimensionException(HERE) << "Sample returned by PythonRandomVector has incorrect size. Got " << sample.getSize() << ". Expected" << size;
    }
  }
  else
  {
    sample = RandomVectorImplementation::getSample(size);
  }
  return sample;
}


/* Mean accessor */
Point PythonRandomVectorImplementation::getMean() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod ( pyObj_,
                               const_cast<char *>( "getMean" ),
                               const_cast<char *>( "()" ) ));
  if ( result.isNull() )
  {
    handleException();
  }

  Point mean(convert<_PySequence_, Point>(result.get()));
  if (mean.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Mean returned by PythonRandomVector has incorrect dimension. Got " << mean.getDimension() << ". Expected" << getDimension();
  return mean;
}

/* Covariance accessor */
CovarianceMatrix PythonRandomVectorImplementation::getCovariance() const
{
  ScopedPyObjectPointer result(PyObject_CallMethod ( pyObj_,
                               const_cast<char *>( "getCovariance" ),
                               const_cast<char *>( "()" ) ));
  if ( result.isNull() )
  {
    handleException();
  }

  CovarianceMatrix covariance(convert<_PySequence_, CovarianceMatrix>(result.get()));
  if (covariance.getDimension() != getDimension()) throw InvalidDimensionException(HERE) << "Covariance matrix returned by PythonRandomVector has incorrect dimension. Got " << covariance.getDimension() << ". Expected" << getDimension();

  return covariance;
}

/* Method save() stores the object through the StorageManager */
void PythonRandomVectorImplementation::save(Advocate & adv) const
{
  RandomVectorImplementation::save( adv );

  pickleSave(adv, pyObj_);
}


/* Method save() reloads the object from the StorageManager */
void PythonRandomVectorImplementation::load(Advocate & adv)
{
  RandomVectorImplementation::load( adv );

  pickleLoad(adv, pyObj_);
}


END_NAMESPACE_OPENTURNS
